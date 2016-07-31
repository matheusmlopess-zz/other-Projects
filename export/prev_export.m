function [resposta2, saidas, errors, previsao_final, prev_real_exp] = prev_export()
global dados_rl;  global dados_trn;   global targets_exp; global input_exp;

% conjunto de inputs exp      %  #
% Vento (D+1)	Exportação (D-1)  	[2]Preço (D)                [3]Preço (D-6)              [4]Hora 
input_exp = dados_trn(:,[40,11,4,2,3])';  %  #
targets_exp=dados_trn(:,13)';                        %  #


% conjunto de inputs de teste hidrica          %  #
input_test = dados_rl(:,[21,6,1,25,26])';     %  #

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
[net1,tr1] = train(net1,input_exp,targets_exp);
[net2,tr2] = train(net2,input_exp,targets_exp);
[net3,tr3] = train(net3,input_exp,targets_exp);
[net4,tr4] = train(net4,input_exp,targets_exp);
[net5,tr5] = train(net5,input_exp,targets_exp);

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
 prev_real_exp=dados_rl(:,6);                                       %  #
 errors = gsubtract(prev_real_exp,previsao_final);     %  #

    bb=[previsao_final prev_real_exp];  graficoPrev(bb)
    figure, ploterrhist(errors)
    
 global previsao_final_expor=previsao_final;
global previsao_dada_expor=prev_real_exp;

previsao_final_expor=
previsao_dada_expor=
% View the Network
%  view(net1)

 % Plots
%    figure, plotperform(tr1)
%    figure, plottrainstate(tr1)
% 
%    figure, ploterrhist(errors)
%     figure, plotmatrix(dados_trn(:,[40,11,4,2,3])) %  treino     %  #
%     figure, plotmatrix(dados_rl(:,[21,6,1,25,26])); %  validação %  #
 resposta2 = 'previsao exportação completa ';   %  #
%  
%   
%   xlswrite('results_matheus.xlsx', saidas, 'results_exp', 'A4');
%   xlswrite('results_matheus.xlsx', prev_real_exp, 'results_exp', 'P4');
%   xlswrite('results_matheus.xlsx', previsao_final, 'results_exp', 'Q4');
  