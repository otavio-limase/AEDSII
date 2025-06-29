/*
1 - Definir a estrutura Registro
2 - Inicialização
3 - Inserção
4 - Inserção ordenada
5 - Remoção
6 - Pesquisa
7 - Impressão dos registros inseridos
8 - Impressão dos registros livre
9 - Impressão da estrutura
*/
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

/*definição da estrutura a ser armazenado no arquivo*/
struct dados
{
    int chave;
    char nome[30];
    // outros dados. Obs: não é recomendável usar tipo string dentro de union
};

union celula
{
    struct
    {
        int quant, first, last, free, tam; // tam armazena o tamanho total da lista
    } cabecalho;
    struct
    {
        int next, prev;
        dados reg;
    } lista;
};

void inicializar(fstream &arq, int n)
{
    /*Inicializando a estrutura*/

    cout << endl
         << endl
         << "Inicializando a estrutura de registros" << endl
         << endl;

    celula cab, l; // cab - acessar a union com dados do cabeçalho. l acessar a union com dados da lista

    /*Posicionando o ponteiro de escrita no início do arquivo*/
    arq.seekp(0, arq.beg);

    /*preenchendo o cabeçalho*/
    cab.cabecalho.quant = 0;
    cab.cabecalho.first = -1;
    cab.cabecalho.last = -1;
    cab.cabecalho.free = 1;
    cab.cabecalho.tam = n;

    /*Gravando o cabeçalho*/
    arq.write((char *)&cab, sizeof(cab));

    /*Gravando os n registros*/
    for (int i = 1; i <= cab.cabecalho.tam; i++)
    {
        if (i == n)
            l.lista.next = -1;
        else
            l.lista.next = i + 1;

        arq.write((char *)&l, sizeof(l));
    }
}

void imprimirEstrutura(fstream &arq)
{
    celula cab, l; // cab - acessar a union com dados do cabeçalho. l acessar a union com dados da lista

    cout << endl
         << endl
         << "Imprimindo a estrutura de registros" << endl;

    /*posicionando o ponteiro de leitura no início do arquivo*/
    arq.seekg(0, arq.beg);

    /*lendo o cabeçalho*/
    arq.read((char *)&cab, sizeof(cab));

    /*imprimindo o cabeçalho*/
    cout << "Cabecalho:\n";
    cout << "==========\n";
    cout << "Quant: " << cab.cabecalho.quant << endl;
    cout << "First: " << cab.cabecalho.first << endl;
    cout << "Last: " << cab.cabecalho.last << endl;
    cout << "Free: " << cab.cabecalho.free << endl;
    cout << "Tamanho: " << cab.cabecalho.tam << endl;

    /*lendo e imprimindo os n registros*/
    cout << "\n\nCelulas da lista:\n";
    cout << "=================\n\n";
    for (int i = 1; i <= cab.cabecalho.tam; i++)
    {
        cout << "Celula posicao " << i << ":\n";
        cout << "-------------------" << endl;
        arq.read((char *)&l, sizeof(l));
        cout << "Reg.chave: " << l.lista.reg.chave << endl;
        cout << "Reg.nome: " << l.lista.reg.nome << endl;
        cout << "Next: " << l.lista.next << endl;
        cout << "Prev: " << l.lista.prev << endl
             << endl;
    }
}

void imprimirLista(fstream &arq)
{
    celula cab, l; // cab - acessar a union com dados do cabeçalho. l acessar a union com dados da lista

    cout << endl
         << endl
         << "Imprimindo registros validos" << endl;

    /*posicionando o ponteiro de leitura no início do arquivo*/
    arq.seekg(0, arq.beg);

    /*lendo o cabeçalho*/
    arq.read((char *)&cab, sizeof(cab));

    /*imprimindo o cabeçalho*/
    cout << "Quantidade de itens: " << cab.cabecalho.quant << endl;

    /*lendo e imprimindo os registros com dados*/
    cout << "\nRegistros:\n\n";
    if (cab.cabecalho.first == -1)
    {
        cout << "Nao existe registros\n";
        return;
    }

    /*posiciona o ponteiro de leitura no primeiro registro apontado por first*/
    arq.seekg(cab.cabecalho.first * sizeof(cab), arq.beg);

    do
    {
        /*lendo o registro*/
        arq.read((char *)&l, sizeof(l));

        /*imprimindo o registro*/
        cout << "----------------------" << endl;
        cout << "Chave: " << l.lista.reg.chave << endl;
        cout << "Nome: " << l.lista.reg.nome << endl
             << endl;

        /*localizando o próximo registro*/
        if (l.lista.next != -1)
            arq.seekg(l.lista.next * sizeof(l), arq.beg);

    } while (l.lista.next != -1);
}

void inserir(fstream &arq, dados d)
{
    celula l, cab;
    int free;

    cout << endl
         << endl
         << "Inserindo a chave " << d.chave << endl;

    /*posiciona o ponteiro de leitura no início do arquivo*/
    arq.seekg(0, arq.beg);

    /*le o cabeçalho*/
    arq.read((char *)&cab, sizeof(cab));

    /*posiciona o ponteiro de leitura na primeira posição livre no arquivo*/
    arq.seekg(sizeof(cab) * cab.cabecalho.free, arq.beg);

    /*le as informações do registro livre*/
    arq.read((char *)&l, sizeof(l));

    /*grava em free o próximo registro livre*/
    free = l.lista.next;

    /*atribuindo as informações do novo registro*/
    l.lista.reg = d;
    l.lista.next = -1;
    l.lista.prev = cab.cabecalho.last;

    /*atualizando o next do registro que vai apontar para o novo registro l*/
    if (l.lista.prev != -1)
    {
        celula bob;

        /*posiciona o ponteiro de leitura para o registro que vai apontar para o novo registro l*/
        arq.seekg(sizeof(l) * l.lista.prev, arq.beg);

        /*lendo o registro*/
        arq.read((char *)&bob, sizeof(bob));

        /*atualizando o next*/
        bob.lista.next = cab.cabecalho.free;

        /*posiciona o ponteiro de escrita para o registro que vai apontar para o novo registro l*/
        arq.seekp(sizeof(l) * l.lista.prev, arq.beg);

        /*gravando o registro*/
        arq.write((char *)&bob, sizeof(bob));
    }

    /*posiciona o ponteiro de leitura para a posição do novo registro l*/
    arq.seekp(sizeof(cab) * cab.cabecalho.free, arq.beg);

    /*gravando o registro*/
    arq.write((char *)&l, sizeof(l));

    /*Atualizando o cabeçalho*/
    if (cab.cabecalho.first == -1)
        cab.cabecalho.first = cab.cabecalho.free;

    cab.cabecalho.quant++;
    cab.cabecalho.last = cab.cabecalho.free;
    cab.cabecalho.free = free;

    /*posiciona o ponteiro de escrita no início do arquivo*/
    arq.seekp(0, arq.beg);

    /*gravado o cabeçalho*/
    arq.write((char *)&cab, sizeof(cab));
}

void imprimirFree(fstream &arq)
{
    celula l, cab;

    /*posiciona o ponteiro de leitura no início do arquivo*/
    arq.seekg(0, arq.beg);
    arq.read((char *)&cab, sizeof(cab));

    // Verifica se a lista não está cheia
    if (cab.cabecalho.free != (-1))
    {
        cout << endl
             << endl
             << "Imprimindo registros livres " << endl;
        // Instancia a variável free e salva o valor do primeiro reg livre
        int free = cab.cabecalho.free;
        while (free != -1)
        {
            arq.seekg(sizeof(cab) * free, arq.beg);
            arq.read((char *)&l, sizeof(l));
            // Lê e imprime o registro free
            cout << "Reg.chave: " << l.lista.reg.chave << endl;
            cout << "Reg.nome: " << l.lista.reg.nome << endl;
            cout << "Next: " << l.lista.next << endl;
            cout << "Prev: " << l.lista.prev << endl
                 << endl;
            // Pega o próximo reg free
            free = l.lista.next;
        }
    }
    else
        // Imprime se a lista estiver cheia
        cout << "Lista cheia";
}

void inserirOrdenado(fstream &arq, dados d)
{
    celula cab, l, anterior, proximo, novo;

    // Lê o cabeçalho
    arq.seekg(0, arq.beg);
    arq.read((char *)&cab, sizeof(cab));

    // Se a lista está vazia, chama função insere já existente e sai da função
    if (cab.cabecalho.first == -1)
    {
        inserir(arq, d);
        return;
    }

    // Se a lista não está vazia, informa que está inserindo ordenado
    cout << "Inserindo Ordenado";
    // Salva posição do primeiro nó e do ant para "iterar" no while
    int pos = cab.cabecalho.first;
    int pos_ant = -1;

    // Importante fazer uma dupla condição de parada aqui!

    while (pos != -1)
    {
        arq.seekg(pos * sizeof(celula), arq.beg);
        arq.read((char *)&l, sizeof(l));

        // Se não colocar o break vai entar em loop infinito
        if (d.chave < l.lista.reg.chave)
            break;

        // Atualiza variáveis
        pos_ant = pos;
        pos = l.lista.next;
    }

    // Pega o primeiro registro livre através do cabeçalho
    arq.seekg(cab.cabecalho.free * sizeof(celula), arq.beg);
    arq.read((char *)&novo, sizeof(novo));
    // Armazena o next do registro free para att cabeçalho
    int prox_free = novo.lista.next;
    // Define novo registro
    novo.lista.reg = d;
    novo.lista.prev = pos_ant;
    novo.lista.next = pos;

    // Ajusta o next do anterior caso tenha
    if (pos_ant != -1)
    {
        arq.seekg(pos_ant * sizeof(celula), arq.beg);
        arq.read((char *)&anterior, sizeof(anterior));
        anterior.lista.next = cab.cabecalho.free;
        arq.seekp(pos_ant * sizeof(celula), arq.beg);
        arq.write((char *)&anterior, sizeof(anterior));
    }
    else
    {
        // Inseriu no início
        cab.cabecalho.first = cab.cabecalho.free;
    }

    // Se tem próximo, ajusta o prev
    if (pos != -1)
    {
        arq.seekg(pos * sizeof(celula), arq.beg);
        arq.read((char *)&proximo, sizeof(proximo));
        proximo.lista.prev = cab.cabecalho.free;
        arq.seekp(pos * sizeof(celula), arq.beg);
        arq.write((char *)&proximo, sizeof(proximo));
    }
    else
    {
        cab.cabecalho.last = cab.cabecalho.free;
    }

    // Grava o novo
    arq.seekp(cab.cabecalho.free * sizeof(celula), arq.beg);
    arq.write((char *)&novo, sizeof(novo));

    // Atualiza cabeçalho
    cab.cabecalho.quant++;
    cab.cabecalho.free = prox_free;

    arq.seekp(0, arq.beg);
    arq.write((char *)&cab, sizeof(cab));

    cout << "Chave " << d.chave << " inserida";
}

bool pesquisa(fstream &arq, dados &d, int &posicao)
{
    /*pesquisa a chave contida na variável d. Se encontrar retorna em d as informações
    encontradas no registro e retorna true*/
    /*Comentario do professor*/

    /*Foi passado um parametro a mais(pos) para passar como referencia a posição e utilizar em outras funções*/

    celula cabecalhoaux;
    celula registro;
    // variaveis para leitura dos dados, 100% jesus

    /*posicionando o ponteiro de leitura no início do arquivo*/
    arq.seekg(0, arq.beg);

    /*lendo o cabeçalho*/
    arq.read((char *)&cabecalhoaux, sizeof(cabecalhoaux));

    int seguinte = cabecalhoaux.cabecalho.first;
    /*Seguinte sera usado inicialmente para pegar a primeira posição e será usado eventualmente para percorrer a lista*/

    while (seguinte != -1)
    {
        /*Laço para percorrer a lista e verificar se encontrou o valor*/

        arq.seekg(sizeof(celula) * seguinte, arq.beg);
        /*posiciona o ponteiro de leitura na primeira posição livre no arquivo*/

        arq.read((char *)&registro, sizeof(registro));
        /*lendo o registro*/
        if (registro.lista.reg.chave == d.chave)
        {
            d = registro.lista.reg;

            posicao = seguinte;
            /*Passando como referencia para usar a posição em outras funções eventualmente*/

            return true;
            /*verifica se é igual, e copiando para as variaveis de saida*/
        }
        seguinte = registro.lista.next;
    }
    return false;
}

bool remover(fstream &arq, dados &d)
{
    /*pesquisa a chave contida na variável d. Se encontrar retorna em d as informações
     encontradas no registro e remove o registro da lista*/
    /*comentario do professor*/

    celula cabecalhoaux;
    celula registro;
    int posicao;
    /*variaveis para leitura dos dados, 100% jesus*/

    /*posicionando o ponteiro de leitura no início do arquivo*/
    arq.seekg(0, arq.beg);

    /*lendo o cabeçalho*/
    arq.read((char *)&cabecalhoaux, sizeof(cabecalhoaux));

    if (!pesquisa(arq, d, posicao))
    {
        return false;
    }

    /*Ler o registro que sera removido, usando a posição da função pesquisa*/
    arq.seekg(sizeof(celula) * posicao, arq.beg);
    arq.read((char *)&registro, sizeof(registro));

    /*se tiver anterior, atualiza o next dele*/
    if (registro.lista.prev != -1)
    {
        celula anterior;

        /*se tiver, carrega esse registro anterior*/
        arq.seekg(sizeof(celula) * registro.lista.prev, arq.beg);
        arq.read((char *)&anterior, sizeof(anterior));

        /*atualiza o ponteiro next do anterior pra pular o registro que será removido*/
        anterior.lista.next = registro.lista.next;

        /* grava o anterior de volta com o next atualizado*/
        arq.seekp(sizeof(celula) * registro.lista.prev, arq.beg);
        arq.write((char *)&anterior, sizeof(anterior));
    }
    else
    {
        /*se não tem anterior, quer dizer que é o primeiro da lista*/
        cabecalhoaux.cabecalho.first = registro.lista.next;
    }

    /*se tiver próximo, atualiza o prev dele*/
    if (registro.lista.next != -1)
    {
        celula proximo;

        /*Carregar o proximo registro*/
        arq.seekg(sizeof(celula) * registro.lista.next, arq.beg);
        arq.read((char *)&proximo, sizeof(proximo));

        /*Novamente, atualiza o prev do proximo pra pular o registro removido*/
        proximo.lista.prev = registro.lista.prev;

        /*grava o próximo de volta com o prev atualizado*/
        arq.seekp(sizeof(celula) * registro.lista.next, arq.beg);
        arq.write((char *)&proximo, sizeof(proximo));
    }
    else
    {
        /*se nao tem proximo, é o último da lista*/
        cabecalhoaux.cabecalho.last = registro.lista.prev;
    }

    // Coloca o registro removid no início da lista de livres
    registro.lista.next = cabecalhoaux.cabecalho.free;
    cabecalhoaux.cabecalho.free = posicao;

    // Atualiza a quantidade de registros válidos
    cabecalhoaux.cabecalho.quant--;

    /* Grava o registro com next atualizado no lugar antigo*/
    arq.seekp(sizeof(celula) * posicao, arq.beg);
    // registro.lista.reg.chave = (-1);
    dados reset;
    registro.lista.reg = reset;
    arq.write((char *)&registro, sizeof(registro));

    /* Grava o novo cabeçalho*/
    arq.seekp(0, arq.beg);
    arq.write((char *)&cabecalhoaux, sizeof(cabecalhoaux));

    /*No final, tudo certo e votando true*/
    return true;
}

int main()
{
    dados d;
    fstream arq;
    int n, op = -1;

    /*Abrindo o arquivo*/
    cout << "Abrindo arquivo pagina.dat ..." << endl;
    arq.open("pagina.dat", ios::binary | fstream::in | fstream::out);
    if (!arq.is_open())
    {
        cout << "Arquivo pagina.dat nao existe!" << endl;
        arq.open("pagina.dat", ios::binary | fstream::in | fstream::out | fstream::trunc);
        if (!arq.is_open())
        {
            cout << "Erro ao abrir o arquivo!!";
            return 0;
        }
        cout << "Criando arquivo pagina.dat ..." << endl;
        cout << "Digite o numero maximo de registros a serem armazenados no arquivo: ";
        cin >> n;
        inicializar(arq, n);
    }

    cout << "Arquivo pagina.dat aberto." << endl;

    /*Menu de opções*/
    while (op != 0)
    {
        cout << "\n\nMenu\n=================\n";
        cout << "1 - Insere" << endl;
        cout << "2 - Insere ordenado" << endl;
        cout << "3 - Remove" << endl;
        cout << "4 - Pesquisa" << endl;
        cout << "5 - Imprime Lista" << endl;
        cout << "6 - Imprime a Estrutura da lista" << endl;
        cout << "7 - Imprime a Lista Free" << endl;
        cout << "0 - Sair" << endl;
        cout << "Digite uma opcao: ";
        cin >> op;
        cout << endl;

        switch (op)
        {
        case 1: /*insere*/
            cout << "Digite a chave do registro a ser inserido: ";
            cin >> d.chave;
            cout << "Digite o nome: ";
            cin >> d.nome;
            inserir(arq, d);
            break;

        case 2: /*insere ordenado*/
            cout << "Digite a chave do registro a ser inserido: ";
            cin >> d.chave;
            cout << "Digite o nome: ";
            cin >> d.nome;
            inserirOrdenado(arq, d);
            break;

        case 3: /*remover*/
            cout << "Digite a chave a ser removida: ";
            cin >> d.chave;
            if (remover(arq, d))
            {
                cout << "\nRegistro removido:\n";
                cout << "Chave: " << d.chave << endl;
                cout << "Nome: " << d.nome << endl;
            }
            else
                cout << "\nRegistro com a chave " << d.chave << " nao encontrado.";
            break;

        case 4:
            cout << "Digite a chave a ser pesquisada: ";
            cin >> d.chave;
            // Adiciona parametro pos que foi inserido na função pesquisa para uso na func remove
            int pos;
            if (pesquisa(arq, d, pos))
            {
                cout << "Chave: " << d.chave << endl;
                cout << "Nome: " << d.nome << endl;
            }
            else
                cout << "\nRegistro com a chave " << d.chave << " nao encontrado.";
            break;

        case 5:
            imprimirLista(arq);
            break;

        case 6:
            imprimirEstrutura(arq);
            break;

        case 7:
            imprimirFree(arq);
            break;

        case 0:
            cout << "\n\nFinalizando o programa\n";
            break;

        default:
            cout << "Opcao Invalida!!!" << endl;
        }
    }

    return 0;
}
