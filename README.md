# other-Projects


C++, MySQL & MatLab Projects

## Project descriptions:
#1- Forecast model using neural networks (Matlab)

In this work, it was considered a data set production schedules, consumption and market prices of the Iberian electricity system. Based on hourly data, during 2012 and 2013, is intended to create predictive models for the following day (D + 1).

The ultimate goal was present the forecasts of the following day (D + 1) for the year 2014 and compere with the real data to validate the forecasts of the following variables:

- Electricity consumption Forecast
- Wind production forecast
- Hydric production forecast
- Cogeneration production forecast + solar
- Thermal Nuclear forecast
- Thermal coal forecast
- Thermal combined cycle forecast
- Exports forecast
- Price forecast MIBEL


#2- Quiz game in Portuguese

##FUNÇÕES DE IDENTIFICAÇÃO

\register <username> <password> - Verifica se o utilizador existe no servidor. Caso isso não se verifique, cria-o na base de dados e associa a password ao mesmo.

\friendlist <username> - Lista os amigos associados a um utilizador.

\identify <username> - Verifica se o utilizador existe na base de dados e se já está ligado. Se ainda não existir, o username passa a estar associado a este utilizador. Serve para as pessoas se poderem ligar sem terem de criar uma conta.

FUNÇÕES INTERNAS PARA FUNCIONAMENTO DO JOGO

\shutdown - Comando para desligar o servidor

\kick <username> - Comando para desligar utilizador do servidor.

\help <comando> - Especifica a função de um comando de ajuda específico.

\help - Lista os comandos possíveis, também acessível aos utilizadores.

\create <questão> - Permite adição de novas questões na base de dados.

\login <username> <password> - Verifica a existência do utilizador na base de dados, em caso afirmativo guarda todas as informações relativas às atividades do utilizador.

\logout <username> - Desliga o utilizador do servidor.

FUNÇÕES MULTI-JOGADOR

\accept <username> - Aceita o jogo de um adversário. Se houver mais do que um desafio, escreve o utilizador que se quer defrontar. Após aceitar, envia mensagem aos outros utilizadores, dizendo que há um jogo a decorrer, entre utilizador A e B.

\friend <username> - Adiciona utilizador a uma lista de amigos.

\decline <username> - Rejeita adversário.

\start - Comando para dar início à partida.

\challenge <username> - Função que permite ao utilizador desafiar outro utilizador.

\info <username> - Verifica se o utilizador está na base de dados, retornando informação de dados, tal como o no de jogos e no de vitórias.

\top10 <username> - Organiza as pontuações do utilizador e retorna as 10 melhores pontuações.

FUNÇÕES DE SUPORTE AO JOGADOR

\fifty_fifty - Função que permite suporte à resposta do utilizador reduzindo as opções de resposta para metade.

\change - Função que permite ao utilizador trocar de questão caso pretenda.

