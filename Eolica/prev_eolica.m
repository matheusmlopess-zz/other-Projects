function [resposta1, saidas, errors, previsao_final, prev_real_eolica] = prev_eolica()
global dados_rl;  global dados_trn;   global targets_eolica; global input_eolica;

global previsao_final_eolica;
global previsao_dada_eolica;

% conjunto de inputs eolica 
 %[40]Vento (D+1)		[43]Pluviosid (D+1)   	[39]Temp (D+1 )	  [16]Eólica (D-1) 	[4]Hora    	[41]Direção (D+1)
input_eolica = dados_trn(:,[40,43,39,16,4,41])'; 
targets_eolica=dados_trn(:,18)';

 %[21]Vento (D+1)		[24]Pluviosid (D+1)   	[20]Temp (D+1 )	  [9]Eólica (D-1) 	[1]Hora    	[22]Direção (D+1)
% conjunto de inputs de teste eolica 
input_test = dados_rl(:,[21,24,20,9,1,22])';

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
[net1,tr1] = train(net1,input_eolica,targets_eolica);
[net2,tr2] = train(net2,input_eolica,targets_eolica);
[net3,tr3] = train(net3,input_eolica,targets_eolica);
[net4,tr4] = train(net4,input_eolica,targets_eolica);
[net5,tr5] = train(net5,input_eolica,targets_eolica);

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
 prev_real_eolica=dados_rl(:,9);

    errors = gsubtract(prev_real_eolica,previsao_final);
    bb=[previsao_final prev_real_eolica];  graficoPrev(bb)
    figure, ploterrhist(errors)
    
    previsao_final_eolica = previsao_final;
    previsao_dada_eolica = prev_real_eolica;
% View the Network
%  view(net1)

 % Plots
%    figure, plotperform(tr1)
%    figure, plottrainstate(tr1)

%     figure, plotmatrix(dados_trn(:,[40,43,39,16,4,41])) %  treino 
%     figure, plotmatrix(dados_rl(:,[21,24,20,9,1,22])); %  validação
    
 resposta1 = 'previsao eolica completa ';
 G=[previsao_final prev_real_eolica];  graficoPrev(G)
%    xlswrite('results_matheus.xlsx', saidas, 'results_eolica', 'A4');
%   xlswrite('results_matheus.xlsx', prev_real_eolica, 'results_eolica', 'P4');
%   xlswrite('results_matheus.xlsx', previsao_final, 'results_eolica', 'Q4');