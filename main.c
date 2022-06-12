#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#define CAB '1' // Define oq é o cabeçalho da informação
#define DATA '2' // define oq é informação

struct boleto // estrutura do boleto
{
    char cliente[6];
    char conta[7];
    char numboleto[9];
    char datavenc[9];
    char datapag[9];
    char valorpag[9];
};


int main()
{
    FILE *rem = fopen("Remessa.txt", "r"); // abre o arquivo de remessa
    FILE *clt; // abre o arquivo do cliente
    FILE *ArvErro = NULL; // abre o arquivo de erro
    int quantidadeLinha = 0; // quantidade de linha atual
    char NomeDoArquivo[20]; // variavel temporaria para guardar o nome do arquivo
    unsigned long long int Soma = 0; // soma do cliente atual 
    unsigned int qtdBolClientes = 0; // quantidade de boletos


    int size = 0; // tamanho da linha lida
    char dataremessa[9]; // dia da remessa
    char chr; // variavel que recebe o primeiro caractere

    struct boleto *Boletos = NULL; // array dinamico de boletos
    struct boleto tempBoleto; // variavel temporaria do boleto
    char ClienteAtual[6]; // numero do cliente atual

    int qtdboletos = 0; // quantidade de boletos lidos
    int error = 0; // flag de erro
    int Avaliar = 0; // flag de avaliação

    int i, j; // variaveis usadadas para organizar os boletos

    while(1)
    {
        chr = fgetc(rem); // pega o primeiro caractere

        if(Avaliar) // caso tenha lido uma linha de informação de boletos
        {
            if( (chr == '\n' || chr == EOF)) // se o próximo caractere for uma nova linha ou fim de arquivo a informação está correta
            {
                Boletos = realloc(Boletos, sizeof(struct boleto) * (qtdboletos+1)); // realoca o array de boletos
                Boletos[qtdboletos] = tempBoleto; // iguala o boleto temporario ao boleto que acabou de ser criado
                qtdboletos++; // aumenta a quantidade de boletos lida
                if (chr == EOF) break; // caso sejá um fim de linha termina o programa 
            }
            else error = 1; // caso n seja uma nova linha levanta a flag de erro indicando que a linha tem muitos caracteres
            Avaliar = 0; // faz com que a avaliação resete
            continue; // pula a leitura do caractere atual
        }

        if(error) // se for uma condição de erro
        {
            if (ArvErro == NULL) ArvErro = fopen("ERROR.txt", "w"); // caso n tenha sido criado, cria o arquivo de log de erros
            fprintf(ArvErro, "Erro na linha: %i \n", quantidadeLinha); // printa a informação para localizar o erro
            if(quantidadeLinha == 1) {fclose(ArvErro); exit(1);}
            if (chr != '\n') while (chr != '\n' && chr != EOF) chr = fgetc(rem); // caso ainda tenha mais caracteres, passa até checar em uma nova linha
            
            error = 0; // reseta a variavel de erro
        }
        else if(chr == '1') // caso sejá um informação do tipo cabeçalho
        {
            fscanf(rem, "%8s", dataremessa); // lê os 8 caracteres e armazena na data de remessa
            size = strlen(dataremessa); // le quantos foram lidos
            quantidadeLinha++; // aumenta a quantidade de linhas lidas

            if(size != 8) {error = 1; continue;}
            
        }
        else if(chr == '2') // caso seja uma informação do tipo boleto
        {
            // carrega os valores para a variavel tempraria e ativa o bit de avaliação
            // cehca tbm se os valores lidos correspondem ao tamanho ideal
            quantidadeLinha++; // aumenta a quantidade de linhas lidas

            fscanf(rem, "%5s", tempBoleto.cliente);
            size = strlen(tempBoleto.cliente);
            if(size != 5) {error = 1; continue;}

            fscanf(rem, "%6s", tempBoleto.conta);
            size = strlen(tempBoleto.conta);
            if(size != 6) {error = 1; continue;}

            fscanf(rem, "%8s", tempBoleto.numboleto);
            size = strlen(tempBoleto.numboleto);
            if(size != 8) {error = 1; continue;}

            fscanf(rem, "%8s", tempBoleto.datavenc);
            size = strlen(tempBoleto.datavenc);
            if(size != 8) {error = 1; continue;}

            fscanf(rem, "%8s", tempBoleto.datapag);
            size = strlen(tempBoleto.datapag);
            if(size != 8) {error = 1; continue;}

            fscanf(rem, "%8s", tempBoleto.valorpag);
            size = strlen(tempBoleto.valorpag);
            if(size != 8) {error = 1; continue;}

            Avaliar = 1;
        }
    }

    // organização dos clientes no array de boletos
    for (i = 0; i < qtdboletos; i++)
    {
        for (j = 0; j < qtdboletos-i-1; j++)
        {
            if (strcmp(Boletos[j].cliente, Boletos[j+1].cliente) > 0)
            {
                tempBoleto = Boletos[j];
                Boletos[j] = Boletos[j+1];
                Boletos[j+1] = tempBoleto;
            }
        }
    }

    // caso n tenham boletos quita o programa
    if (qtdboletos < 1) exit(1);
    // Se for aberto arquivo de erro fecha
    if (ArvErro != NULL) fclose(ArvErro); 
    
    // copia o primeiro cliente e cria seu arquivo
    strcpy(ClienteAtual, Boletos[0].cliente);
    sprintf(NomeDoArquivo, "%s_%s.txt", dataremessa, ClienteAtual);
    clt = fopen(NomeDoArquivo, "w");
    fprintf(clt, "1%s\n", dataremessa);

    // passa pelos boletos e salva as informações do cliente
    for (i = 0; i < qtdboletos; i++)
    {
        if(strcmp(ClienteAtual, Boletos[i].cliente) != 0) // caso o cliente mude é feito a criação do novo arquivo
        {
            if(Soma > 999999999) Soma = 999999999; // cehca se a soma passou do valor maximo
            fprintf(clt, "3%06i%010i\n", qtdBolClientes, Soma); // fecha o arquivo do cliente com o rodapé
            qtdBolClientes = 0; // reseta a quantidade de boletos para o proximo cliente
            Soma = 0; // reseta a somatoria para o proximo cliente
            fclose(clt); // fecha o arquivo do cliente atual

            strcpy(ClienteAtual, Boletos[i].cliente); // atualiza o cliente atual
            sprintf(NomeDoArquivo, "%s_%s.txt", dataremessa, ClienteAtual); // cria o arquivo 
            clt = fopen(NomeDoArquivo, "w"); // abre o aquivo do clriente
            fprintf(clt, "1%s\n", dataremessa); // printa o cabeçalho
        }
        fprintf(clt, "2%s%s%s%s\n", Boletos[i].conta, Boletos[i].numboleto, Boletos[i].datapag, Boletos[i].valorpag); // printa as inrofmações
        qtdBolClientes++; // soma a quantidade de boletos 
        Soma += atoi(Boletos[i].valorpag); // faz a somatoria dos boletos
    }

    // fecha o ultimo cliente
    if(Soma > 999999999) Soma = 999999999; 
    fprintf(clt, "3%06i%010i\n", qtdBolClientes, Soma);
    fclose(clt);
}