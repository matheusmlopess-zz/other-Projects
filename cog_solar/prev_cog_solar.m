function [resposta4, saidas, errors, previsao_final, prev_real_coj_sol] = prev_cog_solar()
global dados_rl;  global dados_trn;   global targets_cog_solar; global input_cog_solar;

% conjunto de inputs coj +solar      %  #
% Hora	Cogeração+Solar (D-1)	Cogeração+Solar (D-6)	Temperatura (D+1)	Irradiância (D+1)	Mês 24

input_cog_solar = dados_trn(:,[8,25,26,39,42])';  %  #
targets_cog_solar=dados_trn(:,24)';             %  #

% conjunto de inputs de teste cog. +solar  %  #

% Hora	Cogeração+Solar (D-1)	Cogeração+Solar (D-6)	Temperatura (D+1)	Irradiância (D+1)	Mês 24

input_test = dados_rl(:,[5,12,13,20,23])';    %  #

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
[net1,tr1] = train(net1,input_cog_solar,targets_cog_solar);
[net2,tr2] = train(net2,input_cog_solar,targets_cog_solar);
[net3,tr3] = train(net3,input_cog_solar,targets_cog_solar);
[net4,tr4] = train(net4,input_cog_solar,targets_cog_solar);
[net5,tr5] = train(net5,input_cog_solar,targets_cog_solar);

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
 prev_real_coj_sol=dados_rl(:,12);                                     %  #
 errors = gsubtract(prev_real_coj_sol,previsao_final);     %  #
  xlswrite('results.xlsx', previsao_final, 'results', 'A4')
    bb=[prev_real_coj_sol previsao_final];  graficoPrev(bb)
    figure, ploterrhist(errors)

     global previsao_final_cog;
global previsao_dada_cog;
  previsao_final_cog = previsao_final ;
 previsao_dada_cog =prev_real_coj_sol ;
 
    % View the Network
%  view(net1)

 % Plots
%    figure, plotperform(tr1)
%    figure, plottrainstate(tr1)
% 
%    figure, ploterrhist(errors)
%    figure, plotmatrix(dados_trn(:,[4,5,6,7,14,15,16,21,25,27,29,33,39,42])) %  treino     %  #
%    figure, plotmatrix(dados_rl(:,[1,2,3,4,7,8,9,10,12,13,14,16,23])); %  validação %  #
 resposta4 = 'previsao cogeração + solar completa ';   %  #
 
%   xlswrite('results_matheus.xlsx', saidas, 'results_exp', 'A4');
%   xlswrite('results_matheus.xlsx', prev_real_coj_sol, 'results_exp', 'P4');
%   xlswrite('results_matheus.xlsx', previsao_final, 'results_exp', 'Q4');
 