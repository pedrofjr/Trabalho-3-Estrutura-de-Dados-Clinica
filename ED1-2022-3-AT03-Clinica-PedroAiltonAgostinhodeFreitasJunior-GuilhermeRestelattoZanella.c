#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct NoLista* PtrNoLista;

typedef struct NoLista {
    int chave;
    char dados[100];
    PtrNoLista proximo;
    PtrNoLista anterior;
} NoLista;

typedef struct {
    PtrNoLista inicio;
    int tamanho;
} ListaOrdenada;

void iniciaListaOrdenada(ListaOrdenada *lista) {
    PtrNoLista sentinela = malloc(sizeof(NoLista));
    sentinela->chave = 0;
    lista->inicio = sentinela;
    lista->tamanho = 0;
}

int tamanhoListaOrdenada(ListaOrdenada *lista) {
    return(lista->tamanho);
}

bool estaVaziaListaOrdenada(ListaOrdenada *lista) {
    return(tamanhoListaOrdenada(lista) == 0);
}

void imprimeListaOrdenada(ListaOrdenada *lista, FILE *fl2) {
    PtrNoLista aux;
    for(aux = lista->inicio->proximo; aux!=lista->inicio; aux=aux->proximo) {
        fprintf(fl2, "%s", aux->dados);
    }
}

void imprimeListaInvertida(ListaOrdenada *lista, FILE *fl2){
    PtrNoLista aux;
    for(aux = lista->inicio->anterior; aux!=lista->inicio; aux=aux->anterior) {
        fprintf(fl2, "%s", aux->dados);
    }
}

void inserirListaOrdenada(ListaOrdenada *lista, int valor, char *linha) {
    PtrNoLista novo = malloc(sizeof(NoLista));
    novo->chave = valor;
    strcpy(novo->dados, linha);
    //Primeira inserção
    if(estaVaziaListaOrdenada(lista)) {
        lista->inicio->proximo = novo;
        lista->inicio->anterior = novo;
        novo->proximo = lista->inicio;
        novo->anterior= lista->inicio;
    }
    //Inserir antes do primeiro valor
    else if(valor < lista->inicio->proximo->chave) {
        lista->inicio->proximo->anterior=novo;
        novo->proximo = lista->inicio->proximo;
        novo->anterior= lista->inicio;
        lista->inicio->proximo = novo;
    }
    //Inserir no meio ou no final da lista
    else {
    PtrNoLista aux = lista->inicio->proximo;
        while(aux->proximo != lista->inicio && valor > aux->proximo->chave) {
            aux = aux->proximo;
        }
        novo->proximo = aux->proximo;
        novo->anterior= aux;
        aux->proximo = novo;
        if(novo->proximo==lista->inicio){
            lista->inicio->anterior=novo;
        }
        else
            novo->proximo->anterior = novo;
    }
    lista->tamanho++;
}

bool removerListaOrdenada(ListaOrdenada *lista, int valor) {
    PtrNoLista rm;
    if(estaVaziaListaOrdenada(lista) || valor<lista->inicio->proximo->chave || valor>lista->inicio->anterior->chave) {
        return false;
    }
    //Remover primeiro elemento da lista
    if(valor == lista->inicio->proximo->chave) {
        rm = lista->inicio->proximo;
        lista->inicio->proximo = lista->inicio->proximo->proximo;
        free(rm);
        if(lista->inicio->proximo==lista->inicio){
            lista->inicio->anterior=lista->inicio;
        }
        else
            lista->inicio->proximo->anterior=lista->inicio;
        lista->tamanho--;
        return true;
    }
    //Remover elemento no meio ou final da lista
    PtrNoLista aux = lista->inicio->proximo;
    while(aux->proximo != lista->inicio && valor > aux->proximo->chave) {
        aux = aux->proximo;
    }
    if(aux->proximo == lista->inicio || aux->proximo->chave > valor) {
        return false;
    }
    rm = aux->proximo;
    aux->proximo = aux->proximo->proximo;
    free(rm);
    lista->tamanho--;
    if(aux->proximo==lista->inicio){
        lista->inicio->anterior=aux;
    }
    else
        aux->proximo->anterior=aux;
    return true;
}

void destruirListaDinamica(ListaOrdenada *lista){
    PtrNoLista aux;
    //Se a lista não estiver vazia, remover todos os elementos
    if(!estaVaziaListaOrdenada(lista)){
        for(aux=lista->inicio->proximo; aux!=lista->inicio; aux=lista->inicio->proximo) {
            removerListaOrdenada(lista,aux->chave);
        }
    }
    //Remover nó sentinela
    removerListaOrdenada(lista,lista->inicio->chave);
}

void pesquisaListaDinamica(ListaOrdenada *lista, int valor, FILE *fl2){
    PtrNoLista aux;
    //Caso o valor seja maior ou igual ao menor elemento e maior ou igual ao último elemento, pesquisar
    if(valor>=lista->inicio->proximo->chave && valor<=lista->inicio->anterior->chave){
        //Percorrer lista até encontrar nó sentinela ou valor maior que o pesquisado
        for(aux = lista->inicio->proximo; aux!=lista->inicio && aux->chave<=valor; aux=aux->proximo) {
            if(aux->chave==valor){
                fprintf(fl2, "%s", aux->dados);
                exit(0);
            }
        }
    }
    //Caso não encontrar elemento pesquisado, imprimir erro no arquivo
    fprintf(fl2, "Elemento não encontrado!\n", valor);
}

int main(int argc, const char *argv[]){
    //Verificação do Argc
    if(argc!=3){
        printf("Quantidade de parametros invalidos: %d\n", argc);
        return 1;
    }
    //Abertura de arquivos de leitura e saída
    FILE *fl1, *fl2;
    fl1=fopen(argv[1], "r");
    fl2=fopen(argv[2], "w");
    if(fl1 == NULL || fl2 == NULL){
        printf("Deu B.O nos arquivos!!!\n");
        return 2;
    }
    
    ListaOrdenada lista;
    iniciaListaOrdenada(&lista);

    char contador[100], linha[100], id[5];
	int chave, operacao, qtdeLinhas=0, LinhaAtual=0, verificacao=0, pesquisa;

    //Conta quantas linhas tem o arquivo
    while(fgets(contador, 100, fl1) != NULL){
		if(contador[0] != '\n'){
			qtdeLinhas++;
		}
	}

    //Retorna ao início do arquivo para pode ler novamente
    fseek(fl1, 0, SEEK_SET);

	while(fgets(linha, 100, fl1) != NULL){
        
        //Conta qual linha do arquivo está sendo lida
        LinhaAtual++;

		//Testa se a linha é de informações do paciente, se é o número de uma operação
		//ou se é uma entrada invalida
		if(linha[0] == 123 && linha[strlen(linha)-2] == 125){

			//Testa se o numero de virgulas é 6
			int virgula = 0;
			for(int i = 0; i <= strlen(linha); i++){
				if (linha[i] == 44){
					virgula++;
				}
			}
			if(virgula != 6){
				fprintf(fl2, "Arquivo Inválido!\n");
				destruirListaDinamica(&lista);
				return 3;
			}

			//Definindo a chave
			id[0] = linha[1];
			id[1] = linha[2];
			chave =  atoi(id);
                
            inserirListaOrdenada(&lista, chave, linha);

		}else if(linha[0] > 47 && linha[0] < 58){
            
			//Verifica se a operação é 1, 2 ou 3 
            //Caso seja 3, define o número que vai ser pesquisado
            if(LinhaAtual < qtdeLinhas){
                if(linha[0] == '3'){
                    operacao = atoi(linha);
                    verificacao += 1;
                }else{
                    fprintf(fl2, "Arquivo Inválido!\n");
                    destruirListaDinamica(&lista);
			        return 4;
                }
            }else{
                if(verificacao == 0){
                    if(linha[0] == '1' || linha[0] == '2'){
                        operacao = atoi(linha);
                    }else{
                        fprintf(fl2, "Arquivo Inválido!\n");
                        destruirListaDinamica(&lista);
                        return 5;
                    }
                }else if(verificacao == 1){
                    pesquisa = atoi(linha);
                }
            }
		}else{
			fprintf(fl2, "Arquivo Inválido!\n");
			destruirListaDinamica(&lista);
			return 6;
		}
	}

    //Chamar as operações
    if (operacao == 1){
        imprimeListaOrdenada(&lista, fl2);
	} else if (operacao == 2){
        imprimeListaInvertida(&lista, fl2);
	} else if (operacao == 3){
        pesquisaListaDinamica(&lista, pesquisa, fl2);
    }
    
	//Destruir pilha e liberar memória alocada
    destruirListaDinamica(&lista);

    //Fechar arquivos e retornar código de controle de erros
    fclose(fl1);
    fclose(fl2);
    return 0;
}