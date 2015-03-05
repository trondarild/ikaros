%%%% plain demo that when a RNN is driven by different signals, the induced
%%%% internal signals will inhabit different subspaces of the signal space.


% set figure window to 1 x 2 panels


set(0,'DefaultFigureWindowStyle','docked');

%%% Experiment control
randstate = 8; newNets = 1; newSystemScalings = 1;
linearMorphing = 0;

%%% Setting system params
Netsize = 100; % network size
NetSpectralRadius = 1.5; % spectral radius - magnitude of largest eigenvalue
NetinpScaling = 1.5; % scaling of pattern feeding weights
BiasScaling = 0.2; % size of bias


%%% loading learning
TychonovAlpha = .0001; % regularizer for  W training
washoutLength = 500; % why is this necessary? What determines size?
learnLength = 1000; % what determines size?
signalPlotLength = 20;

%%% pattern readout learning
TychonovAlphaReadout = 0.01;


%%% C learning and testing
alpha = 10;
ConceptorTestLength = 400;
SplotLength = 50;

%%% Setting patterns

patterns = [53 75 76 36];

% 1 = sine10  2 = sine15  3 = sine20  4 = spike20
% 5 = spike10 6 = spike7  7 = 0   8 = 1
% 9 = rand5; 10 = rand5  11 = rand6 12 = rand7
% 13 = rand8 14 = sine10range01 15 = sine10rangept5pt9
% 16 = rand3 17 = rand9 18 = rand10 19 = 0.8 20 = sineroot27
% 21 = sineroot19 22 = sineroot50 23 = sineroot75
% 24 = sineroot10 25 = sineroot110 26 = sineroot75tenth
% 27 = sineroots20plus40  28 = sineroot75third
% 29 = sineroot243  30 = sineroot150  31 = sineroot200
% 32 = sine10.587352723 33 = sine10.387352723
% 34 = rand7  35 = sine12  36 = 10+perturb  37 = sine11
% 38 = sine10.17352723  39 = sine5 40 = sine6
% 41 = sine7 42 = sine8  43 = sine9 44 = sine12
% 45 = sine13  46 = sine14  47 = sine10.8342522
% 48 = sine11.8342522  49 = sine12.8342522  50 = sine13.1900453
% 51 = sine7.1900453  52 = sine7.8342522  53 = sine8.8342522
% 54 = sine9.8342522 55 = sine5.19004  56 = sine5.8045
% 57 = sine6.49004 58 = sine6.9004 59 = sine13.9004
% 60 = 18+perturb
% 75 = triangle wave
% 76 = square wave

%%% Initializations ==================================================

randn('state', randstate); % normal distribution
rand('twister', randstate); % flat distribution

% Create raw weights
if newNets
    if Netsize <= 20
        Netconnectivity = 1;
    else
        Netconnectivity = 10/Netsize;
    end
    WstarRaw = generate_internal_weights(Netsize, Netconnectivity); % Wstar means W is adaptive/learned
    WinRaw = randn(Netsize, 1);
    WbiasRaw = randn(Netsize, 1);
end

% Scale raw weights and initialize weights - why is this necessary?
if newSystemScalings
    Wstar = NetSpectralRadius * WstarRaw;
    Win = NetinpScaling * WinRaw;
    Wbias = BiasScaling * WbiasRaw;
end

% Set pattern handles
pattHandles; 
%%% in order to get same patterns as in the hierarchical
%architecture demo, run that demo first and do not call pattHandles here
%again. 

I = eye(Netsize);

% % learn equi weights

% harvest data from network externally driven by patterns
% initialize collectors
Np = length(patterns);
allTrainArgs = zeros(Netsize, Np * learnLength);
allTrainOldArgs = zeros(Netsize, Np * learnLength);
% allTrainTargs = zeros(Netsize, Np * learnLength);
allTrainOuts = zeros(1, Np * learnLength);
readoutWeights = cell(1,Np);
patternCollectors = cell(1,Np);

SRCollectors = cell(1,Np);
URCollectors = cell(1,Np);
patternRs = cell(1,Np);
train_xPL = cell(1,Np);
train_pPL = cell(1,Np);
startXs = zeros(Netsize, Np);
Cs = cell(4, Np);

% collect data from driving native reservoir with different drivers
for p = 1:Np
    patt = patts{patterns(p)}; % get current pattern generator
    
    % set up collectors for reservoir
    xCollector = zeros(Netsize, learnLength ); % used to collect states of network during length of learning, and then to calculate conceptor
    xPrevCollector = zeros(Netsize, learnLength );
    pCollector = zeros(1, learnLength ); % save input here
    x = zeros(Netsize, 1); % netsize rows, 1 column - this is the actual reservoir network
    
    % first do washout then store states of the network during pattern
    % driving - no learning yet
    for n = 1:(washoutLength + learnLength)
        u = patt(n); % get pattern input
        xOld = x;
        % update x, state of network
        x = tanh(Wstar * x + Win * u + Wbias);
        
        if n > washoutLength % store values after washout
            xCollector(:, n - washoutLength ) = x; % will contain all x vectors for the whole learn length
            xPrevCollector(:, n - washoutLength ) = xOld; % why this?
            pCollector(1, n - washoutLength) = u; % store pattern signal
        end
    end
    
    % calculate the correlation matrix
    R = xCollector * xCollector' / learnLength;
    patternRs{p} = R;
    R = patternRs{p}; % get R matrix for this pattern
    [U S V] = svd(R);
    % actually compute conceptor here? why not use R directly?
    % this only computes the sigma part - to be able to plot later?
    Snew = (S * inv(S + alpha^(-2) * eye(Netsize)));
    % actually compute the conceptor
    C = U * Snew * U';
    
    % store (for plotting?)
    Cs{1, p} = C;
    Cs{2, p} = U;
    Cs{3, p} = diag(Snew); % singular values with aperture
    Cs{4, p} = diag(S); % singular values without aperture
    
    

    % calculate singular value decomposition
    [Ux Sx Vx] = svd(R);
    % store the singular values for this pattern
    SRCollectors{1,p} = Sx;
    URCollectors{1,p} = Ux;

    
    
    startXs(:,p) = x;
    train_xPL{1,p} = xCollector(:,1:signalPlotLength);
    train_pPL{1,p} = pCollector(1,1:signalPlotLength);
    
    patternCollectors{1,p} = pCollector;
    allTrainArgs(:, (p-1)*learnLength+1:p*learnLength) = ...
        xCollector;
    allTrainOldArgs(:, (p-1)*learnLength+1:p*learnLength) = ...
        xPrevCollector;
    allTrainOuts(1, (p-1)*learnLength+1:p*learnLength) = ...
        pCollector;
    % allTrainTargs(:, (p-1)*learnLength+1:p*learnLength) = ...
    %    Win * pCollector;
end % end of pattern loop

%%% compute W and Wout ==================================================

% this is the ridge regression step

%%% compute W: first get target, thensize do ridge regression
% do inverse tanh, and subtract bias (since do this on the weight and 
% network values.
% note: have to do this after have collected states for all patterns?
% want to do it incrementally (but probably have to use other method then)
Wtargets = (atanh(allTrainArgs) - repmat(Wbias,1,Np*learnLength));

W = (inv(allTrainOldArgs * allTrainOldArgs' + ...
    TychonovAlpha * eye(Netsize)) * allTrainOldArgs * Wtargets')';
    
tst1 =     size(inv(allTrainOldArgs * allTrainOldArgs' + ...
    TychonovAlpha * eye(Netsize)))
    
tst2 =     size(allTrainOldArgs * Wtargets')

% training errors per neuron
NRMSE_W = nrmse(W*allTrainOldArgs, Wtargets);
disp(sprintf('mean NRMSE W: %g', mean(NRMSE_W)));

% note: why does Wout use allTrainArgs, but W uses allTrainOldArgs
%       Wout is dimension [1, N]
Wout = (inv(allTrainArgs * allTrainArgs' + ...
    TychonovAlphaReadout * eye(Netsize)) ...
    * allTrainArgs * allTrainOuts')';
    
tst3 = size(inv(allTrainArgs * allTrainArgs' + ...
    TychonovAlphaReadout * eye(Netsize)))
tst4 = size(allTrainArgs * allTrainOuts')
    
% training error - normalized root mean squared error
NRMSE_readout = nrmse(Wout*allTrainArgs, allTrainOuts); % size(allTrainOuts) = 4*learnlegnth because 4 patterns
disp(sprintf('NRMSE readout: %g', NRMSE_readout));

%%% run loaded reservoir to observe a messy output. Do this with starting
%%% from four states originally obtained in the four driving conditions
%%% --note--: think this is output with oscillations that are not filtered with 
%%% conceptors, but which has updated W matrix - should allow network to
%%% oscillate between all states
%%% note: see that network stabilizes at y=1.5 after about 50 steps - why?
%%% Because learnlength = 50? No
%%
figure(10); clf;
% initialize network state
for p = 1:4
    x = startXs(:,p);
    messyOutPL = zeros(1,ConceptorTestLength);
    % run
    for n = 1:ConceptorTestLength
        x = tanh(W*x + Wbias); % using updated W and Wout
        y = Wout * x;
        messyOutPL(1,n) = y;
    end
    subplot(2,2,p);
    plot(messyOutPL(1,1:end));
end





% % test with applying C to network
x_CTestPL = zeros(5, ConceptorTestLength, Np);
p_CTestPL = zeros(1, ConceptorTestLength, Np);
figure(11); clf;
% test OR AND
%for p = 1:Np
    p = 1
    C = Cs{1, p};
    C2 = Cs{1, 2};
    C3 = Cs{1, 3};
    x = startXs(:,p);
    % x = 0.5*randn(Netsize,1);
    Cfirst = C3;%AND( NOT(AND(NOT(C3), C)), C2);
    Csecond = (AND(NOT(C3), C));
    plotlen = 100;
    for n = 1:ConceptorTestLength
        x = tanh(W *  x + Wbias); % note: use updated W here!
        % simple concatenation
        if n<plotlen;
            Ctst = Cfirst;
        else
            Ctst = Csecond;
        end
        x = Ctst * x; % apply conceptor
        x_CTestPL(:,n,p) = x(1:5,1);
        p_CTestPL(:,n,p) = Wout * x;
    end
    subplot(2,2,p);
    plot(p_CTestPL(1,1:plotlen,p));
%end


%%% plotting =======================================================



test_pAligned_PL = cell(1,Np);
test_xAligned_PL = cell(1,Np);
NRMSEsAligned = zeros(1,Np);
MSEsAligned = zeros(1,Np);

for p = 1:Np
    intRate = 20;
    thisDriver = train_pPL{1,p};
    thisOut = p_CTestPL(1,:,p);
    thisDriverInt = interp1((1:signalPlotLength)',thisDriver',...
        (1:(1/intRate):signalPlotLength)', 'spline')';
    thisOutInt = interp1((1:ConceptorTestLength)', thisOut',...
        (1:(1/intRate):ConceptorTestLength)', 'spline')';
    
    L = size(thisOutInt,2); M = size(thisDriverInt,2);
    phasematches = zeros(1,L - M);
    for phaseshift = 1:(L - M)
        phasematches(1,phaseshift) = ...
            norm(thisDriverInt - ...
            thisOutInt(1,phaseshift:phaseshift+M-1));
    end
    [maxVal maxInd] = max(-phasematches);
    test_pAligned_PL{1,p} = ...
        thisOutInt(1,maxInd:intRate:...
        (maxInd+intRate*signalPlotLength-1));
    coarseMaxInd = ceil(maxInd / intRate);
    test_xAligned_PL{1,p} = ...
        x_CTestPL(:,coarseMaxInd:coarseMaxInd+signalPlotLength-1,p);
    NRMSEsAligned(1,p) = ...
        nrmse(test_pAligned_PL{1,p},train_pPL{1,p});
    MSEsAligned(1,p) = ...
        mean((test_pAligned_PL{1,p} - train_pPL{1,p}).^2 );
end

meanNRMSE = mean(NRMSEsAligned)

%%
figure(2); clf;
fs = 24; fsNRMSE = 18;

set(gcf, 'WindowStyle','normal');

set(gcf,'Position', [600 400 1000 500]);

pick1 = 71; pick2 = 80;
col1 = 0.6*[1 1 1]; col2 = 0.3*[1 1 1];
for p = 1:Np
    subplot(Np,4,(p-1)*4+1);
    plot(test_pAligned_PL{1,p}, 'LineWidth',10,...
        'Color',0.75*[1 1 1]); hold on;
    plot(train_pPL{1,p},'k','LineWidth',1.5); hold off;
    if p == 1
        title('p and y','FontSize',fs);
    end
    if p ~= Np
        set(gca, 'XTickLabel',[]);
    end
    set(gca, 'YLim',[-1,1], 'FontSize',fs);

    
    subplot(Np,4,(p-1)*4+2); hold on;
    plot(train_xPL{1,p}(pick1,:)','Color',col1,'LineWidth',3);
    plot(train_xPL{1,p}(pick2,:)','Color',col2,'LineWidth',3);
    hold off;
    if p == 1
        title('two neurons','FontSize',fs);
    end
    if p ~= Np
        set(gca, 'XTickLabel',[]);
    end
    set(gca,'YLim',[-1,1], 'FontSize',fs, 'Box', 'on');
    
    subplot(Np,4,(p-1)*4+3);
    %diagNormalized = sDiagCollectors{1,p} / sum(sDiagCollectors{1,p});
    hold on;
    plot(log10(diag(SRCollectors{1,p})),'k','LineWidth',3);
    plot(zeros(1,100),'k--');
    hold off;
    
    set(gca,'YLim',[-17,5], 'YTick',[-10, 0], 'FontSize',fs, 'Box','on');
    if p == 1
        title('log10 \sigma','FontSize',fs);
    end
    if p < 4
        set(gca,'XTick',[]);
    end
    
    subplot(Np,4,(p-1)*4+4);
    hold on;
    plot(zeros(1,100),'k--');
    plot(ones(1,100),'k--');
    plot(Cs{3, p},'k','LineWidth',3);
    hold off;
    if p == 1
        title('s','FontSize',fs);
    end
    set(gca,'YLim',[-0.1,1.1], 'YTick',[0 1], 'FontSize',fs, 'Box','on');
    if p < 4
        set(gca,'XTick',[]);
    end
end

%%

col1 = 0.5*[1 1 1]; col2 = 0.7*[1 1 1]; 
col3 = 0.3*[1 1 1]; col4 = [0.6 0. 0.4];
L = 100;
fs = 16;
trace1 = allTrainArgs([pick1 pick2],1:L);
trace2 = allTrainArgs([pick1 pick2],1001:1000+L);
trace3 = allTrainArgs([pick1 pick2],2001:2000+L);
trace4 = allTrainArgs([pick1 pick2],3001:3000+L);
R1 = trace1 * trace1' / L; [U1 S1 V1] = svd(R1);
R2 = trace2 * trace2' / L; [U2 S2 V2] = svd(R2);
R3 = trace3 * trace3' / L; [U3 S3 V3] = svd(R3);
R4 = trace4 * trace4' / L; [U4 S4 V4] = svd(R4);
cycleData = [cos(2 * pi * (0:200) / 200 ); sin(2 * pi * (0:200) / 200) ];
E1 = R1 * cycleData; E2 = R2 * cycleData;
figure(3); clf;
set(gcf, 'WindowStyle','normal');
set(gcf,'Position', [800 200 600 200]);
rg = 1.2;
subplot(1,3,1);
hold on;
plot([-rg rg], [0 0], 'k--');
plot([0 0], [-rg rg], 'k--');
plot(cycleData(1,:), cycleData(2,:), 'k','LineWidth',1.5);
plot(trace1(1,:), trace1(2,:), '.','Color',col1, 'MarkerSize', 20);

plot(E1(1,:), E1(2,:), 'Color',col1, 'LineWidth',2);
plot(S1(1,1) * [0,U1(1,1)], S1(1,1) * [0,U1(2,1)],'Color',col1, 'LineWidth',2);
plot(S1(2,2) * [0,U1(1,2)], S1(2,2) * [0,U1(2,2)],'Color',col1, 'LineWidth',2);

hold off;
set(gca, 'XLim', [-rg rg], 'YLim', [-rg rg], 'XTick',[-1 0 1],...
   'YTick',[-1 0 1], 'FontSize', fs, 'Box', 'on' );
axis square;

a = 1.6;
C1 = R1 * inv(R1 + a^(-2)*eye(2));
[U1c S1c V1c] = svd(C1);
C2 = R2 * inv(R2 + a^(-2)*eye(2));
[U2c S2c V2c] = svd(C2);
C3 = R3 * inv(R3 + a^(-2)*eye(2));
[U3c S3c V3c] = svd(C3);
C4 = R4 * inv(R4 + a^(-2)*eye(2));
[U4c S4c V4c] = svd(C4);
E1c = C1 * cycleData; E2c = C2 * cycleData;
E3c = C3 * cycleData; E4c = C4 * cycleData;

subplot(1,3,2);
hold on;
plot([-rg rg], [0 0], 'k--');
plot([0 0], [-rg rg], 'k--');
plot(cycleData(1,:), cycleData(2,:), 'k','LineWidth',1.5);
plot(E1(1,:), E1(2,:), 'Color',col1, 'LineWidth',1.5);
plot(E1c(1,:), E1c(2,:), 'Color',col3, 'LineWidth',4);
plot(S1(1,1) * [0,U1(1,1)], S1(1,1) * [0,U1(2,1)],'Color',col1, ...
    'LineWidth',1.5);
plot(S1(2,2) * [0,U1(1,2)], S1(2,2) * [0,U1(2,2)],'Color',col1, ...
    'LineWidth',1.5);
plot(S1c(1,1) * [0,U1c(1,1)], S1c(1,1) * [0,U1c(2,1)],'Color',col3, ...
    'LineWidth',4);
plot(S1c(2,2) * [0,U1c(1,2)], S1c(2,2) * [0,U1c(2,2)],'Color',col3,...
    'LineWidth',4);

set(gca, 'XLim', [-rg rg], 'YLim', [-rg rg], 'XTick',[-1 0 1],...
   'YTick',[-1 0 1], 'FontSize', fs, 'Box', 'on' );
axis square;

subplot(1,3,3);
ms = 20; lw = 1.5;
hold on;
plot([-rg rg], [0 0], 'k--');
plot([0 0], [-rg rg], 'k--');
plot(cycleData(1,:), cycleData(2,:), 'k','LineWidth',1.5);
plot(E1c(1,:), E1c(2,:), 'Color',col1, 'LineWidth',lw);
plot(E2c(1,:), E2c(2,:), 'Color',col3, 'LineWidth',lw);
plot(S1c(1,1) * [0,U1c(1,1)], S1c(1,1) * [0,U1c(2,1)],'Color',col1, ...
    'LineWidth',lw);
plot(S1c(2,2) * [0,U1c(1,2)], S1c(2,2) * [0,U1c(2,2)],'Color',col1, ...
    'LineWidth',lw);
plot(S2c(1,1) * [0,U2c(1,1)], S2c(1,1) * [0,U2c(2,1)],'Color',col3, ...
    'LineWidth',lw);
plot(S2c(2,2) * [0,U2c(1,2)], S2c(2,2) * [0,U2c(2,2)],'Color',col3, ...
    'LineWidth',lw);

hold off;
set(gca, 'XLim', [-rg rg], 'YLim', [-rg rg], 'XTick',[-1 0 1],...
   'YTick',[-1 0 1], 'FontSize', fs, 'Box', 'on' );
axis square;
