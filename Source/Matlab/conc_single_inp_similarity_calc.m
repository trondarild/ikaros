%
% similarity on wave forms test
%
%pkg load image;
randstate = 8;
randn('state', randstate);
rand('twister', randstate);


net_multiplier = 100;

inp_rows = 1; % single input, only one row
netsize = net_multiplier*inp_rows; % five times as large network as dimensional input
pattern_length = 300;

% set up patterns
% want a sine function, triangle, and square
% with 2 different frequencies and 2 different amplitudes
triwave = @(amp, freq, n) amp*8/pi^2 * (sin(2*pi*n*freq) ...
    - 1/9*sin(3*2*pi*n *freq)...
    + 1/25*sin(5*2*pi*n*freq)...
    - 1/49*sin(7*2*pi*n*freq)...
    + 1/89*sin(9*2*pi*n*freq));
squarewave = @(amp, freq, n) amp*4/pi * (sin(	 2*pi*n*freq) ...
 								 + 1/3*sin(3*2*pi*n*freq)...
 								 + 1/5*sin(5*2*pi*n*freq)...
 								 + 1/7*sin(7*2*pi*n*freq)...
 								 + 1/9*sin(9*2*pi*n*freq)...
 							  + 1/11*sin(11*2*pi*n*freq));
sinwave = @(amp, freq, n) amp*sin(2*pi*n*freq);

shapes{1,1} = sinwave;
shapes{1,2} = triwave;
shapes{1,3} = squarewave;
shapes{2,1} = 'sinwave';
shapes{2,2} = 'triwave';
shapes{2,3} = 'squarewave';

freq_list = [0.01 0.03];
amp_list = [1 5];

funcs = {};
funcs_id = {};
ix = 1;
% now combine into permutations:
for i=1:length(shapes)
	for j=1:length(freq_list)
		for k=1:length(amp_list)
			func = shapes{1,i};
			frq = freq_list(j);
			amp = amp_list(k);
			% store id
			funcs_id{ix,1} = shapes{2,i};
			funcs_id{ix,2} = amp;
			funcs_id{ix,3} = frq;
			
			% store actual function
			funcs{ix++} = @(n) func(amp, frq, n);
		end
	end
end
num_patterns = length(funcs);
% create a list of pairs to do similarity calc on later
similarity_pair_ix = nchoosek(linspace(1, length(funcs), length(funcs)), 2);

% params
netconnectivity = 0.1;
net_spectral_radius = 1.5;
net_input_scaling = 1.5;
bias_scaling = 0.2;

tychonov_alpha = 0.0001;
washout_length = 500;
learnlength = 5*pattern_length; % go through image 5 times
tychonov_alpha_readout = 0.01;

conceptor_aperture = 10;%^1.1;
conceptor_test_length = 400;

% generate W_star, W_in (same dimensionality as input), bias
W_star_raw = generate_internal_weights(netsize, netconnectivity); % Wstar means W is adaptive/learned
W_in_raw = randn(netsize, inp_rows);
bias_raw = randn(netsize, 1);
I = eye(netsize);

% scaling for weight vectors
W_star = W_star_raw * net_spectral_radius;
W_in = W_in_raw * net_input_scaling;
bias = bias_raw * bias_scaling;

% set up collectors
x_current_collector_all = zeros(netsize, num_patterns*learnlength);
x_prev_collector_all = zeros(netsize, num_patterns*learnlength);
input_collector_all = zeros(inp_rows, num_patterns*learnlength);
x_collector = zeros(netsize, learnlength);
pattern_netw_states = zeros(netsize, num_patterns);

conceptors = cell(1,num_patterns);
conceptor_prev = eye(netsize, netsize);

% loop through inputs, store output
for pattern_ix = 1:length(funcs)
	pattern_gen = funcs{pattern_ix};
	% collectors for current pattern
	x_collector = zeros(netsize, learnlength);
	x_prev_collector = zeros(netsize, learnlength);
	x_current_collector = zeros(netsize, learnlength);
	input_collector = zeros(inp_rows, learnlength);

	network_state = zeros(netsize, 1);
   % do washout
   for time= 1:(washout_length + learnlength)
   	% cycle through image in wraparaound fashion
   	inp = pattern_gen(time);
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
   % conceptor_prev = conceptor;
    
end

% calc similarity between all conceptors
for i=1:length(similarity_pair_ix)
	a_ix = similarity_pair_ix(i,1);
	b_ix = similarity_pair_ix(i,2);
	a = conceptors{1, a_ix};
	b = conceptors{1, b_ix};
	similarity = conceptor_similarity(a, b);
	id_a = sprintf('%g,%s,%g', funcs_id{a_ix,2}, funcs_id{a_ix,1}, funcs_id{a_ix,3});
	id_b = sprintf('%g,%s,%g', funcs_id{b_ix,2}, funcs_id{b_ix,1}, funcs_id{b_ix,3});
	% print out (should perhaps go to file)
	% disp(sprintf('first=(%s); second=(%s), similarity=%g', id_a, id_b, similarity));
	disp(sprintf('%s,%s,%g', id_a, id_b, similarity));
end



% do ridge regression =========================
% % updated W
% W_targets = atanh(x_current_collector_all) - repmat(bias, 1, num_patterns*learnlength);
% 
% W_upd = (inv(x_prev_collector_all*x_prev_collector_all' + tychonov_alpha*I)...
%  * x_prev_collector_all*W_targets')';
% 
% NRMSE_W = nrmse(W_upd*x_prev_collector_all, W_targets);
% disp(sprintf('mean NRMSE W: %g', mean(NRMSE_W)));
% 
% % updated W_out
% W_out = (inv(x_current_collector_all*x_current_collector_all' + tychonov_alpha_readout*I)...
%  * x_current_collector_all*input_collector_all')';
% 
% % check RMS error against input
% NRMSE_OUT = nrmse(W_out*x_current_collector_all, input_collector_all);
% disp(sprintf('mean NRMSE readout: %g', mean(NRMSE_OUT)));
% 
%
% plot output and input ==========================
%
