function [resposta3, saidas, errors, previsao_final, prev_real_hidrica] = prev_hidrica()
global dados_rl;  global dados_trn;   global targets_hidrica; global input_hidrica;

% conjunto de inputs hidrica      %  #
% Hora	Consumo (D-1)	Consumo (D-6)	Hídrica (D-1)	Hidrica (D-6) % C(D-1) %/ N(D-1  %GN (D-1)	% [43]Pluviosid (D+1)

input_hidrica = dados_trn(:,[4,14,15,21,22,29,33,37,43])';  %  #
targets_hidrica=dados_trn(:,20)';                        %  #

% Hora	Consumo (D-1)	Consumo (D-6)	Hídrica (D-1)	Hidrica (D-6) % C(D-1) %/ N(D-1  %GN (D-1)	% [43]Pluviosid (D+1)
% conjunto de inputs de teste hidrica          %  #
input_test = dados_rl(:,[1,7,8,10,11,14,16,18,24])';     %  #

% Create a Fitting Network
hiddenLayerSize = 20;
net1 = fitnet(hiddenLayerSize);  net2 = fitnet(hiddenLayerSize);  
net3 = fitnet(hiddenLayerSize);  net4 = fitnet(hiddenLayerSize);
net5 = fitnet(hiddenLayerSize);

net1.divideParam.trainRatio = 80/100;    net1.divideParam.valRatio   = 15/100;    net1.divideParam.testRatio  = 5/100;
net2.divideParam.trainRatio = 80/100;    net2.divideParam.valRatio   = 15/100;   net2.divideParam.testRatio  = 5/100;
net3.divideParam.trainRatio = 80/100;    net3.divideParam.valRatio = 15/100;     net3.divideParam.testRatio = 5/100;
net4.divideParam.trainRatio = 80/100;    net4.divideParam.valRatio = 15/100;     net4.divideParam.testRatio = 5/100;
net5.divideParam.trainRatio = 80/100;    net5.divideParam.valRatio = 15/100;     net5.divideParam.testRatio = 5/100;

% Train the Network
[net1,tr1] = train(net1,input_hidrica,targets_hidrica);
[net2,tr2] = train(net2,input_hidrica,targets_hidrica);
[net3,tr3] = train(net3,input_hidrica,targets_hidrica);
[net4,tr4] = train(net4,input_hidrica,targets_hidrica);
[net5,tr5] = train(net5,input_hidrica,targets_hidrica);

% Test the Network
out1 = net1(input_test);
out2 = net2(input_test);
out3 = net3(input_test);
out4 = net4(input_test);
out5 = net5(input_test);

saidas=[out1;out2;out3;out4;out5];
saidas=saidas';
           previsao_final=saidas(:,1);
           previsao_final=mean(saidas,2);
 prev_real_hidrica=dados_rl(:,10);                                     %  #
 errors = gsubtract(prev_real_hidrica,previsao_final);     %  #

 global previsao_final_hidrica;
global previsao_dada_hidrica;

 previsao_final_hidrica=previsao_final;
 previsao_dada_hidrica=prev_real_hidrica;
    bb=[previsao_final prev_real_hidrica];  graficoPrev(bb)
    figure, ploterrhist(errors)
% View the Network
%  view(net1)

 % Plots
%    figure, plotperform(tr1)
%    figure, plottrainstate(tr1)
% 
%    figure, ploterrhist(errors)
   figure, plotmatrix(dados_trn(:,[4,14,21,22,29,33,37,40,19])) %  treino     %  #
   figure, plotmatrix(dados_rl(:,[1,7,8,10,11,14,16,18,21])); %  validação %  #
 resposta3 = 'previsao hidrica completa ';   %  #
 
%   xlswrite('results_matheus.xlsx', saidas, 'results_hidrica', 'A4');
%   xlswrite('results_matheus.xlsx', prev_real_hidrica, 'results_hidrica', 'P4');
%   xlswrite('results_matheus.xlsx', previsao_final, 'results_hidrica', 'Q4');
  