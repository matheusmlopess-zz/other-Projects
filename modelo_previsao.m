clear all; clc; close all; format longG;
global dados_rl; global dados_trn; global targets_eolica; global input_eolica; global targets_cog_solar; 
global input_eolica__cog_solar; global input_hidrica; global target_hidrica;


global previsao_final_eolica;
global previsao_dada_eolica;

 global previsao_final_expor;
global previsao_dada_expor;

global previsao_final_hidrica;
global previsao_dada_hidrica;

    global previsao_final_cog;
global previsao_dada_cog;


run dados_treino.m ;   run dados_real.m;

 
% Variaveis dados treino <> dados_trn(:,[numereo])
%[1]Pre�o (D+1)               %[2]Pre�o (D)                %[3]Pre�o (D-6)              %[4]Hora                     %[5]DDS (D) 
%[6]DDS (D+1)                 %[7]M�s (D+1)             %[8]ano (D+1)                %[9]Consumo (D)       %[10]Consumo (D+1)   
%[11]Exporta��o (D-1)     %[12]Exporta��o (D)   %[13]Exporta��o (D+1)  %[14]Consumo (D-1)  %[15]Consumo (D-6)
%[16]E�lica (D-1)              %[17]E�lica (D)            %[18]E�lica (D+1)           %[19]H�drica (D)          %[20]H�drica (D+1) 
%[21]H�drica (D-1)            %[22]Hidrica (D-6)       %[23]Cog+Solar (D )      %[24]Cog+Solar (D+1) 
%[25]Coge+Solar (D-1)   %[26]Cog+Solar (D-6)  %[27]Carvao (D)             %[28]Carvao (D+1)	   %[29]Carvao (D-1)           
%[30]Carvao (D-6)            %[31]Nuclear (D)           %[32]Nuclear (D+1)        %[33]Nuclear (D-1)
%[34]Nuclear (D-6)	         %[35]GN (D)                  %[36]GN (D+1)              %[37]GN (D-1)            %[38]GN (D-6)           
%[39]Temp (D+1  )            %[40]Vento (D+1)	     %[41]Dire��o (D+1)      %[42]Irradi�nc (D+1)  %[43]Pluviosid (D+1)

% variaveis dados real <> dados_rl(:,[numereo])
%[1]Hora                      %[2]DDS (D)                        %[3]DDS (D+1)             	  %[4]M�s (D+1)               %[5]ano (D+1)            
%[6]Exporta��o (D-1)  %[7]Consumo (D-1)            %[8]Consumo (D-6)         %[9]E�lica (D-1)	         %[10]H�drica (D-1)
%[11]Hidrica (D-6)       %[12]Cog+Solar (D-1)        %[13]Cogo+Solar (D-6)	 %[14]Carvao (D-1)        %[15]Carvao (D-6)	
%[16]Nuclear (D-1)      %[17]Nuclear (D-6)             %[18]GN (D-1)	               %[19]GN (D-6)              %[20]Tem (D+1)	 
%[21]Vento (D+1)	   %[22]Dire��o (D+1)           %[23]Irradi�ncia (D+1)   %[24]Pluviosidade (D+1)	
%[25]Pre�o (D)             %[26]Pre�o (D-6)              

%Previsoes
% [resposta1, saidas_eolica,errors_eolica,previsao_eolica, prev_real_eolica ]= prev_eolica();       
% Y=[prev_real_eolica previsao_eolica];  graficoPrev(Y)
% disp(resposta1)
%  %------------------------------------------------------------------------------------------------------------------------
%  [resposta2, saidas_expo, errors_exp, previsao_final_exp, prev_real_exp]= prev_export();
% G=[previsao_final prev_real_exp];  graficoPrev(G)
% disp(resposta2)
%  %------------------------------------------------------------------------------------------------------------------------
%  [resposta3, saidas_hidrica, errors_hidrica, previsao_final_hidrica, prev_real_hidrica]  = prev_hidrica(); 
%   W=[previsao_final_hidrica prev_real_hidrica];  graficoPrev(W)
%   disp(resposta3)
 %------------------------------------------------------------------------------------------------------------------------
  [ resposta4, saidas_cogSoerrors, previsao_final_coj_sol, prev_real_coj_sol]   = prev_cog_solar(); 
  Z=[previsao_final_coj_sol prev_real_coj_sol];  graficoPrev(Z)
  disp(resposta4)

% % parte leonel
% 
%  %------------------------------------------------------------------------------------------------------------------------
% %  t�rmica de Carvao
%   [ resposta4, saidas_carvao, previsao_carvao, prev_real_carvao]   = prev_carvao(); 
%  cc=[previsao_carvao prev_real_carvao];  graficoPrev(cc)
%   disp(resposta4)
%  %------------------------------------------------------------------------------------------------------------------------
% %  Nuclear
%   [ resposta4, saidas_nuclear, previsao_final_nuclear, prev_real_nuclear]   = prev_nuclear(); 
%   dd=[previsao_final_nuclear prev_real_nuclear];  graficoPrev(dd)
%   disp(resposta4)
%  %------------------------------------------------------------------------------------------------------------------------
% % GN
%   [ resposta4, saidas_gn, previsao_final_gn, prev_real_gn]   = prev_gn(); 
%   bb=[previsao_final_gn prev_real_gn];  graficoPrev(bb)
%   disp(resposta4)
%  %------------------------------------------------------------------------------------------------------------------------
% % Consumo
%   [ resposta4, saidas_consumo, previsao_final_consumo, prev_real_consumo]   = prev_consumo(); 
%   kk=[previsao_final_consumo prev_real_consumo];  graficoPrev(kk)
%   disp(resposta4)
%  %------------------------------------------------------------------------------------------------------------------------
% %  Pre�o
%   [ resposta4, saidas_preco, previsao_final_preco, prev_real_preco]   = prev_preco(); 
%   rr=[previsao_final_preco prev_real_preco];  graficoPrev(rr)
%   disp(resposta4)
