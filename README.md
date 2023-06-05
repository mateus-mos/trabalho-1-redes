# trabalho-1-redes

Protocolo das mensagens (baseado no Kermit)

- Marcador de início: 8 bits [01111110]
- Tamanho: 6 bits
- Sequência: 4 bits
- Tipo: 4 bits
- Dados: Tamanho em bytes
- Paridade Vertical: 8 bits

## To-do

- [ ] Backup de 1 arquivo
- [ ] Backup mais de um arquivo
- [ ] Recupera 1 arquivo
- [ ] Recupera mais de um arquivo
- [ ] Escolhe diretorio servidor
- [ ] Verifica backup

| Tipo 	|          Descriçao          	|
|:----:	|:---------------------------:	|
| 0000 	|      Backup de 1 arquivo  	  |
| 0001 	|  Backup mais de um arquivo  	|
| 0010 	|      Recupera 1 arquivo     	|
| 0011 	| Recupera mais de um arquivo 	|
| 0100 	|  Escolhe diretorio servidor 	|
| 0101 	|       Verifica backup       	|
| 0110 	|    nome_arquivo recupera    	|
| 0111 	|             MD5             	|
| 1000 	|            Dados            	|
| 1001 	|         Fim arquivo         	|
| 1010 	|      Fim grupo arquivos     	|
| 1011 	|                             	|
| 1100 	|             Erro            	|
| 1101 	|              OK             	|
| 1110 	|             ACK             	|
| 1111 	|             NACK            	|
