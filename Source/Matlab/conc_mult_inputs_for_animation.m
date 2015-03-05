%
% multiple input test
%
%pkg load image;
randstate = 8;
randn('state', randstate);
rand('twister', randstate);
% load image
img_names = cell(1,3);
img_names{1,1} = 'face_1.jpg'; %'minicar.jpg';
img_names{1,2} = 'face_2.jpg'; %'Honda-Pilot.jpg';
img_names{1,3} = 'face_3.png'; %'visiomreveal.jpg';
% img_names{1,1} = 'minicar.jpg';
% img_names{1,2} = 'Honda-Pilot.jpg';
% img_names{1,3} = 'visiomreveal.jpg';
%img_names{1,1} = 'pic_1.jpg';
%img_names{1,2} = 'pic_2.jpg';

image_count = size(img_names,2);
patterns = cell(1, image_count);

% need to scale to avoid saturation and infinity values
input_scale = 0.0875;
for i=1:image_count
	imgname = img_names{1,i};
	img = imread(imgname);
	img = interp1([0 255], input_scale*[-1 1], double(img(:,:,1))); % map to -1..1
	patterns{1,i} = img;
end
[im_rows, im_cols] = size(patterns{1,1}); % get number of rows
net_multiplier = 5;
netsize = net_multiplier*im_rows; % five times as large network as dimensional input




% params
netconnectivity = 0.1;
net_spectral_radius = 1.5;
net_input_scaling = 1.5;
bias_scaling = 0.2;

tychonov_alpha = 0.0001;
washout_length = 500;
learnlength = 5*im_cols; % go through image 5 times
tychonov_alpha_readout = 0.01;

conceptor_aperture = 10;%^1.1;
conceptor_test_length = 400;

% generate W_star, W_in (same dimensionality as input), bias
W_star_raw = generate_internal_weights(netsize, netconnectivity); % Wstar means W is adaptive/learned
W_in_raw = randn(netsize, im_rows);
bias_raw = randn(netsize, 1);
I = eye(netsize);

% scaling for weight vectors
W_star = W_star_raw * net_spectral_radius;
W_in = W_in_raw * net_input_scaling;
bias = bias_raw * bias_scaling;

% set up collectors
x_current_collector_all = zeros(netsize, image_count*learnlength);
x_prev_collector_all = zeros(netsize, image_count*learnlength);
input_collector_all = zeros(im_rows, image_count*learnlength);
x_collector = zeros(netsize, learnlength);
pattern_netw_states = zeros(netsize, image_count);

conceptors = cell(1,image_count);

% loop through inputs, store output
for pattern_ix = 1:image_count
	img_matrix = patterns{1, pattern_ix};
	[img_rows, img_cols] = size(img_matrix);

	% collectors for current pattern
	x_collector = zeros(netsize, learnlength);
	x_prev_collector = zeros(netsize, learnlength);
	x_current_collector = zeros(netsize, learnlength);
	input_collector = zeros(img_rows, learnlength);

	network_state = zeros(netsize, 1);
   % do washout
   for time= 1:(washout_length + learnlength)
   	% cycle through image in wraparaound fashion
   	inp = img_matrix(:,  mod(time-1,img_cols)+1);
		x_prev = network_state;

		% update network state
		network_state = tanh(W_star*network_state + W_in*inp + bias);

		if time > washout_length
			% start actually collecting values
			x_prev_collector(:, time - washout_length) = x_prev;
			x_collector(:, time - washout_length) = network_state;
			input_collector(:, time - washout_length) = inp;
		end
	end
	% save state so can check conceptor reproduction later
	pattern_netw_states(:,pattern_ix) = network_state;
	% calculate conceptor for current pattern
	correlation = x_collector*x_collector'/learnlength;
	conceptor = correlation*inv(correlation+conceptor_aperture^(-2)*I);
	conceptors{1, pattern_ix} = conceptor;

	% collect collectors for current pattern
	x_current_collector_all(:, (pattern_ix-1)*learnlength+1:pattern_ix*learnlength) = ...
	   x_collector;
	x_prev_collector_all(:, (pattern_ix-1)*learnlength+1:pattern_ix*learnlength) = ...
	   x_prev_collector;
   input_collector_all(:, (pattern_ix-1)*learnlength+1 : pattern_ix*learnlength) = ...
   input_collector;
   % do learn-length
end

% do ridge regression =========================
% updated W
W_targets = atanh(x_current_collector_all) - repmat(bias, 1, image_count*learnlength);

W_upd = (inv(x_prev_collector_all*x_prev_collector_all' + tychonov_alpha*I)...
 * x_prev_collector_all*W_targets')';

NRMSE_W = nrmse(W_upd*x_prev_collector_all, W_targets);
disp(sprintf('mean NRMSE W: %g', mean(NRMSE_W)));

% updated W_out
W_out = (inv(x_current_collector_all*x_current_collector_all' + tychonov_alpha_readout*I)...
 * x_current_collector_all*input_collector_all')';

% check RMS error against input
NRMSE_OUT = nrmse(W_out*x_current_collector_all, input_collector_all);
disp(sprintf('mean NRMSE readout: %g', mean(NRMSE_OUT)));

%
% plot output and input ==========================
%
plot_collector = zeros(im_rows, conceptor_test_length);
figure(01); clf;
for pattern_ix = 1:image_count
	% conceptor = conceptors{1, pattern_ix};
	% conceptor = AND(conceptors{1, 1}, conceptors{1, 3});
	% conceptor = NOT(AND(conceptors{1, 1}, conceptors{1, 3}));
	% conceptor = OR(conceptors{1, 1}, conceptors{1, 3});
	conceptor = AND(conceptors{1, 3}, conceptors{1, 3});
	% conceptor = NOT(conceptors{1, pattern_ix});
	network_state = pattern_netw_states(:, pattern_ix);
	% network_state = randn(netsize,1); 

	for time = 1:conceptor_test_length
		network_state = tanh(W_upd*network_state + bias);
		network_state = conceptor * network_state;
		
		plot_collector(:,time) = W_out * network_state;
	end
	subplot(2,2,pattern_ix);
	tmp_img = mat2gray(plot_collector, input_scale*[-1 1]);
	imshow(tmp_img);
	% save to disk
	imwrite(tmp_img, strcat(strcat('img_', num2str(pattern_ix)), '.jpg'));
end
% scratch ============================================


