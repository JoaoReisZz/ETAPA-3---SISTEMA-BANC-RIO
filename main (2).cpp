/******************************************************************************
Trabalho – Implementação de um Sistema de
Registro e Gestão de Contas Bancárias (ETAPA 3)

Aluno: João Lucas Ladeira dos Reis
Matricula: 28509
*******************************************************************************/
#include <iostream>
#include <string>
#include <fstream> // coloquei essa biblioteca pra conseguir ler e salvar nos arquivos .txt
using namespace std;

// Defini o limite máximo de 100 contas e 500 movimentações para caber tudo na memória
const int MAX_CONTAS = 100;
const int MAX_MOVIMENTACOES = 500;

// Fiz essa função pra transformar o texto que vem do arquivo em número inteiro
int converterInteiro(string texto) {
    int num = 0; // acumula o valor conforme vai lendo os caracteres
    for (int i = 0; i < (int)texto.length(); i++) {
    // só pego o que for dígito de 0 a 9
    if (texto[i] >= '0' && texto[i] <= '9') {
        num = num * 10 + (texto[i] - '0'); // empurra uma casa decimal e soma o número atual
        }
    }
    return num;
}

// Essa aqui faz a mesma ideia, mas pra número decimal (tipo saldo e valor de movimentação)
    double converterDecimal(string texto) {
    double valor = 0.0;
    double casaDecimal = 0.1; // começa nos décimos depois que acha o ponto
    bool achouPonto = false; // marca se já passei do ponto decimal

    for (int i = 0; i < (int)texto.length(); i++) {
    if (texto[i] == '.') {
        achouPonto = true; // a partir daqui entra na parte decimal
    } else if (texto[i] >= '0' && texto[i] <= '9') {
    if (!achouPonto) {
        valor = valor * 10.0 + (texto[i] - '0'); // monta a parte inteira
    } else {
        // vai somando cada casa depois do ponto
        valor = valor + (texto[i] - '0') * casaDecimal;
        casaDecimal = casaDecimal * 0.1;
            }
        }
    }
    return valor;
}

// Puxa as contas salvas no contas.txt e joga de volta pros vetores
    void carregarContas(int numeroConta[], string nomeCliente[], string cpf[],
                        int tipoConta[], double saldo[], bool contaAtiva[],
                        int &quantidadeContas) {
    ifstream arquivo("contas.txt"); // abro o arquivo só pra leitura
    quantidadeContas = 0; // zero antes de carregar pra não duplicar nada

    // Se ainda não existe o arquivo (tipo na primeira vez que rodo), só saio e deixo vazio
    if (!arquivo.is_open()) {
        return;
    }

    string linha;
    // Leio linha por linha até acabar o arquivo ou bater o limite de contas
    while (getline(arquivo, linha) && quantidadeContas < MAX_CONTAS) {
        if (linha == "") {
            continue; // ignora linha em branco se tiver
        }

        // Vou achando cada ';' e pegando o pedaço correspondente de cada campo
        int inicio = 0;
        int fim = (int)linha.find(';', inicio);
        numeroConta[quantidadeContas] = converterInteiro(linha.substr(inicio, fim - inicio));

        inicio = fim + 1;
        fim = (int)linha.find(';', inicio);
        nomeCliente[quantidadeContas] = linha.substr(inicio, fim - inicio); // nome pode ter espaço, então pego o texto inteiro até o ';'

        inicio = fim + 1;
        fim = (int)linha.find(';', inicio);
        cpf[quantidadeContas] = linha.substr(inicio, fim - inicio);

        inicio = fim + 1;
        fim = (int)linha.find(';', inicio);
        tipoConta[quantidadeContas] = converterInteiro(linha.substr(inicio, fim - inicio));

        inicio = fim + 1;
        fim = (int)linha.find(';', inicio);
        saldo[quantidadeContas] = converterDecimal(linha.substr(inicio, fim - inicio));

        // O último campo vai até o fim da linha e diz se tá ativa (1) ou não (0)
        inicio = fim + 1;
        string statusTexto = linha.substr(inicio);
        contaAtiva[quantidadeContas] = (converterInteiro(statusTexto) == 1);

        quantidadeContas++; // conto mais uma conta carregada
    }

    arquivo.close(); // fecho o arquivo no final
}

// Grava todas as contas dos vetores dentro do contas.txt
void salvarContas(const int numeroConta[], const string nomeCliente[], const string cpf[],
                  const int tipoConta[], const double saldo[], const bool contaAtiva[],
                  int quantidadeContas) {
    ofstream arquivo("contas.txt"); // abro sem append pra regravar tudo atualizado
    if (!arquivo.is_open()) {
        cout << "Erro ao abrir contas.txt para salvar." << endl;
        return;
    }

    for (int i = 0; i < quantidadeContas; i++) {
        // Converto o bool pra 1 ou 0 na hora de gravar no arquivo
        int status = 0;
        if (contaAtiva[i]) {
            status = 1;
        }

        // Gravo cada conta numa linha separada por ponto e vírgula
        arquivo << numeroConta[i] << ";"
                << nomeCliente[i] << ";"
                << cpf[i] << ";"
                << tipoConta[i] << ";"
                << saldo[i] << ";"
                << status << endl;
    }

    arquivo.close();
}

// Recupera o histórico de movimentações do movimentacoes.txt pros vetores
void carregarMovimentacoes(int contaMovimentacao[], char tipoMovimentacao[],
                           double valorMovimentacao[], int &quantidadeMovimentacoes) {
    ifstream arquivo("movimentacoes.txt");
    quantidadeMovimentacoes = 0;

    // Se ainda não tem histórico salvo, começa zerado mesmo
    if (!arquivo.is_open()) {
        return;
    }

    string linha;
    while (getline(arquivo, linha) && quantidadeMovimentacoes < MAX_MOVIMENTACOES) {
        if (linha == "") {
            continue;
        }

        // Pego o número da conta da movimentação
        int inicio = 0;
        int fim = (int)linha.find(';', inicio);
        contaMovimentacao[quantidadeMovimentacoes] = converterInteiro(linha.substr(inicio, fim - inicio));

        // Pego o tipo ('D' ou 'S')
        inicio = fim + 1;
        fim = (int)linha.find(';', inicio);
        string tipoTexto = linha.substr(inicio, fim - inicio);
        if (!tipoTexto.empty()) {
            tipoMovimentacao[quantidadeMovimentacoes] = tipoTexto[0];
        }

        // Pego o valor até o final da linha
        inicio = fim + 1;
        string valorTexto = linha.substr(inicio);
        valorMovimentacao[quantidadeMovimentacoes] = converterDecimal(valorTexto);

        quantidadeMovimentacoes++;
    }

    arquivo.close();
}

// Salva todo o histórico de movimentações no movimentacoes.txt
void salvarMovimentacoes(const int contaMovimentacao[], const char tipoMovimentacao[],
                           const double valorMovimentacao[], int quantidadeMovimentacoes) {
    ofstream arquivo("movimentacoes.txt");
    if (!arquivo.is_open()) {
        cout << "Erro ao abrir movimentacoes.txt para salvar." << endl;
        return;
    }

    // Gravo cada movimentação numa linha no formato conta;tipo;valor
    for (int i = 0; i < quantidadeMovimentacoes; i++) {
        arquivo << contaMovimentacao[i] << ";"
                << tipoMovimentacao[i] << ";"
                << valorMovimentacao[i] << endl;
    }

    arquivo.close();
}

int main()
{
// Variável que vai guardar a opção que eu digitar no menu
int opcao;

// Vetores para guardar os dados de todas as contas cadastradas
int numeroConta[MAX_CONTAS];
string nomeCliente[MAX_CONTAS];
string cpf[MAX_CONTAS];
int tipoConta[MAX_CONTAS];
double saldo[MAX_CONTAS];
bool contaAtiva[MAX_CONTAS];

// Aqui começa com zero contas cadastradas
int quantidadeContas = 0;

// Vetores para guardar o histórico das movimentações (saques, depósitos, etc.)
int contaMovimentacao[MAX_MOVIMENTACOES];
char tipoMovimentacao[MAX_MOVIMENTACOES];
double valorMovimentacao[MAX_MOVIMENTACOES];
int quantidadeMovimentacoes = 0;

// Logo que o programa abre, já carrego o que tava salvo nos arquivos
carregarContas(numeroConta, nomeCliente, cpf, tipoConta, saldo, contaAtiva, quantidadeContas);
carregarMovimentacoes(contaMovimentacao, tipoMovimentacao, valorMovimentacao, quantidadeMovimentacoes);

// Usei o do-while para o menu rodar pelo menos uma vez e continuar até eu escolher sair
do {
    cout << "***********************" << endl;
    cout << "** MENU - BANCO REIS **" << endl;
    cout << "***********************" << endl;
    cout << "1 - Cadastrar conta" << endl;
    cout << "2 - Listar contas" << endl;
    cout << "3 - Consultar conta" << endl;
    cout << "4 - Realizar deposito" << endl;
    cout << "5 - Realizar saque" << endl;
    cout << "6 - Realizar transferencia" << endl;
    cout << "7 - Exibir extrato" << endl;
    cout << "8 - Alterar tipo da conta" << endl;
    cout << "9 - Ativar/Desativar conta" << endl;
    cout << "10 - Sair" << endl;
    
    cout << "\nDigite uma opcao: ";
    cin >> opcao;
    
    switch(opcao) {
        
        // Opção 1: Aqui faço o cadastro de uma nova conta no sistema
    case 1:
        // Verifico se já bati o limite de 100 contas
        if (quantidadeContas >= MAX_CONTAS) {
            cout << "Limite maximo de contas atingido." << endl;
            break;
        }
            
        cout << "=== CADASTRO DE CONTA ===" << endl;
            
        // Valido se o número da conta é positivo
        do {
            cout << "Digite o numero da conta: ";
            cin >> numeroConta[quantidadeContas];
                
            if (numeroConta[quantidadeContas] <= 0) {
                cout << "Erro: o numero deve ser positivo." << endl;
            }
        } while (numeroConta[quantidadeContas] <= 0);
            
        cin.ignore();
            
        cout << "Digite o nome do titular: ";
        getline(cin, nomeCliente[quantidadeContas]);
            
        cout << "Digite o CPF do titular: ";
        cin >> cpf[quantidadeContas];
            
        // Valido o tipo de conta: 1 para Corrente ou 2 para Poupança
        do {
            cout << "Digite o tipo da conta (1 = Corrente, 2 = Poupanca): ";
            cin >> tipoConta[quantidadeContas];
                
            if (tipoConta[quantidadeContas] != 1 && tipoConta[quantidadeContas] != 2) {
                cout << "Erro: Tipo de conta inexistente." << endl;
            }
        } while (tipoConta[quantidadeContas] != 1 && tipoConta[quantidadeContas] != 2);
            
        cout << "Digite o saldo inicial: ";
        cin >> saldo[quantidadeContas];
            
        // Toda conta nova já nasce ativa por padrão
        contaAtiva[quantidadeContas] = true;
            
        cout << "CONTA CADASTRADA COM SUCESSO." << endl;
        quantidadeContas++;
        // Salvo logo pra conta nova já ficar gravada no contas.txt
        salvarContas(numeroConta, nomeCliente, cpf, tipoConta, saldo, contaAtiva, quantidadeContas);
        break;
            
    // Opção 2: Mostro a lista com todas as contas que já cadastrei
    case 2:
    {
        if (quantidadeContas == 0) {
            cout << "Nenhuma conta cadastrada." << endl;
        break;
        }
                
        cout << "\n----- LISTA DE CONTAS CADASTRADAS -----" << endl;
        for (int i = 0; i < quantidadeContas; i++) {
            cout << "Numero: " << numeroConta[i] << " | Titular: " << nomeCliente[i] << " | Saldo: R$ " << saldo[i];
        if (contaAtiva[i]) {
            cout << " | Status: Ativa" << endl;
        } else {
            cout << " | Status: Desativada" << endl;
                }
            }
        }
        break;
            
    // Opção 3: Procuro uma conta específica pelo número e exibo os dados dela
    case 3:
    {
        if (quantidadeContas == 0) {
            cout << "Nenhuma conta cadastrada." << endl;
        break;
        }
                
        int numeroBusca;
        int posicao = -1;
                
        cout << "Digite o numero da conta para consulta: ";
        cin >> numeroBusca;
                
        for (int i = 0; i < quantidadeContas; i++) {
            if (numeroConta[i] == numeroBusca) {
            posicao = i;
        break;
            }
        }
                
        if (posicao == -1) {
            cout << "Conta nao encontrada." << endl;
        } else {
            cout << "----- DADOS DA CONTA -----" << endl;
            cout << "Numero da conta: " << numeroConta[posicao] << endl;
            cout << "Nome do titular: " << nomeCliente[posicao] << endl;
            cout << "CPF: " << cpf[posicao] << endl;
                    
            if (tipoConta[posicao] == 1) {
                cout << "Tipo: Corrente" << endl;
            } else {
                cout << "Tipo: Poupanca" << endl;
            }
                    
            cout << "Saldo: R$ " << saldo[posicao] << endl;
                    
            if (contaAtiva[posicao]) {
                cout << "Status: Ativa" << endl;
            } else {
                cout << "Status: Desativada" << endl;
                }
            }
        }
        break;
            
        // Opção 4: Faço o depósito somando no saldo e registrando no histórico
    case 4:
        {
        if (quantidadeContas == 0) {
            cout << "Nenhuma conta cadastrada." << endl;
        break;
        }
                
        int numeroBusca;
        int posicao = -1;
        double valor;
                
        cout << "Digite o numero da conta para deposito: ";
        cin >> numeroBusca;
                
        for (int i = 0; i < quantidadeContas; i++) {
            if (numeroConta[i] == numeroBusca) {
            posicao = i;
         break;
            }
        }
                
        if (posicao == -1) {
            cout << "Conta nao encontrada." << endl;
        } else if (!contaAtiva[posicao]) {
            cout << "Erro: A conta esta desativada." << endl;
        } else {
            cout << "Digite o valor do deposito: ";
        cin >> valor;
                    
        if (valor <= 0) {
            cout << "Erro: O valor deve ser positivo." << endl;
        } else if (quantidadeMovimentacoes >= MAX_MOVIMENTACOES) {
            cout << "Erro: Limite de movimentacoes atingido." << endl;
        } else {
        // Atualizo o saldo e jogo o depósito 'D' lá no vetor de histórico
        saldo[posicao] = saldo[posicao] + valor;
                        
        contaMovimentacao[quantidadeMovimentacoes] = numeroConta[posicao];
        tipoMovimentacao[quantidadeMovimentacoes] = 'D';
        valorMovimentacao[quantidadeMovimentacoes] = valor;
        quantidadeMovimentacoes++;

        // Atualizo os dois arquivos pra não perder o saldo novo nem o depósito
        salvarContas(numeroConta, nomeCliente, cpf, tipoConta, saldo, contaAtiva, quantidadeContas);
        salvarMovimentacoes(contaMovimentacao, tipoMovimentacao, valorMovimentacao, quantidadeMovimentacoes);
                        
        cout << "DEPOSITO REALIZADO COM SUCESSO." << endl;
                }
            }
        }
            break;
            
    // Opção 5: Faço o saque conferindo se tenho saldo e registro a saída 'S'
    case 5:
        {
        if (quantidadeContas == 0) {
            cout << "Nenhuma conta cadastrada." << endl;
        break;
        }
                
        int numeroBusca;
        int posicao = -1;
        double valor;
                
        cout << "Digite o numero da conta para saque: ";
        cin >> numeroBusca;
                
        for (int i = 0; i < quantidadeContas; i++) {
            if (numeroConta[i] == numeroBusca) {
            posicao = i;
        break;
            }
        }
                
        if (posicao == -1) {
            cout << "Conta nao encontrada." << endl;
        } else if (!contaAtiva[posicao]) {
            cout << "Erro: A conta esta desativada." << endl;
        } else {
            cout << "Digite o valor do saque: ";
        cin >> valor;
                    
        if (valor <= 0) {
            cout << "Erro: O valor deve ser positivo." << endl;
        } else if (valor > saldo[posicao]) {
            cout << "Erro: Saldo insuficiente." << endl;
        } else if (quantidadeMovimentacoes >= MAX_MOVIMENTACOES) {
            cout << "Erro: Limite de movimentacoes atingido." << endl;
        } else {
        // Desconto do saldo e registro o saque no histórico
        saldo[posicao] = saldo[posicao] - valor;
                        
        contaMovimentacao[quantidadeMovimentacoes] = numeroConta[posicao];
        tipoMovimentacao[quantidadeMovimentacoes] = 'S';
        valorMovimentacao[quantidadeMovimentacoes] = valor;
        quantidadeMovimentacoes++;

        // Salvo o saldo atualizado e o saque nos arquivos
        salvarContas(numeroConta, nomeCliente, cpf, tipoConta, saldo, contaAtiva, quantidadeContas);
        salvarMovimentacoes(contaMovimentacao, tipoMovimentacao, valorMovimentacao, quantidadeMovimentacoes);
                        
        cout << "SAQUE REALIZADO COM SUCESSO." << endl;
                }
            }
        }
        break;
            
    // Opção 6: Faço a transferência entre duas contas gerando dois registros no histórico
    case 6:
        {
        if (quantidadeContas < 2) {
            cout << "Sao necessarias pelo menos duas contas cadastradas para transferencia." << endl;
        break;
        }
                
        int contaOrigem, contaDestino;
        int posOrigem = -1, posDestino = -1;
        double valor;
                
        cout << "Digite o numero da conta de origem: ";
        cin >> contaOrigem;
        cout << "Digite o numero da conta de destino: ";
        cin >> contaDestino;
                
        if (contaOrigem == contaDestino) {
            cout << "Erro: A conta de origem e destino nao podem ser a mesma." << endl;
        break;
        }
                
        // Acho onde estão a conta de origem e a de destino nos vetores
        for (int i = 0; i < quantidadeContas; i++) {
            if (numeroConta[i] == contaOrigem) {
        posOrigem = i;
            }
            if (numeroConta[i] == contaDestino) {
            posDestino = i;
            }
        }
                
        if (posOrigem == -1 || posDestino == -1) {
            cout << "Erro: Uma ou ambas as contas nao foram encontradas." << endl;
        } else if (!contaAtiva[posOrigem] || !contaAtiva[posDestino]) {
            cout << "Erro: Ambas as contas precisam estar ativas." << endl;
        } else {
        cout << "Digite o valor da transferencia: ";
        cin >> valor;
                    
        if (valor <= 0) {
            cout << "Erro: O valor deve ser positivo." << endl;
        } else if (valor > saldo[posOrigem]) {
            cout << "Erro: Saldo insuficiente na conta de origem." << endl;
        } else if (quantidadeMovimentacoes + 2 > MAX_MOVIMENTACOES) {
            cout << "Erro: Espaco insuficiente no historico de movimentacoes." << endl;
        } else {
                
        // Mexo nos saldos: tiro da origem e ponho no destino
        saldo[posOrigem] = saldo[posOrigem] - valor;
        saldo[posDestino] = saldo[posDestino] + valor;
                        
        // Registro a saída na conta de origem ('S')
        contaMovimentacao[quantidadeMovimentacoes] = numeroConta[posOrigem];
        tipoMovimentacao[quantidadeMovimentacoes] = 'S';
        valorMovimentacao[quantidadeMovimentacoes] = valor;
        quantidadeMovimentacoes++;
        
        // Registro a entrada na conta de destino ('D')
        contaMovimentacao[quantidadeMovimentacoes] = numeroConta[posDestino];
        tipoMovimentacao[quantidadeMovimentacoes] = 'D';
        valorMovimentacao[quantidadeMovimentacoes] = valor;
        quantidadeMovimentacoes++;

        // Gravo os dois saldos novos e os dois registros ('S' e 'D') nos arquivos
        salvarContas(numeroConta, nomeCliente, cpf, tipoConta, saldo, contaAtiva, quantidadeContas);
        salvarMovimentacoes(contaMovimentacao, tipoMovimentacao, valorMovimentacao, quantidadeMovimentacoes);
                            
        cout << "TRANSFERENCIA REALIZADA COM SUCESSO." << endl;
                }
            }
        }
        break;
            
    // Opção 7: Exibo o extrato filtrando apenas as movimentações daquela conta
    case 7:
        {
        if (quantidadeContas == 0) {
            cout << "Nenhuma conta cadastrada." << endl;
        break;
        }
                
        int numeroBusca;
        int posicao = -1;
                
        cout << "Digite o numero da conta para exibir o extrato: ";
        cin >> numeroBusca;
            
        for (int i = 0; i < quantidadeContas; i++) {
            if (numeroConta[i] == numeroBusca) {
        posicao = i;
        break;
                }
            }
                
        if (posicao == -1) {
            cout << "Conta nao encontrada." << endl;
        } else {
        cout << "\n===== EXTRATO DA CONTA: " << numeroConta[posicao] << " =====" << endl;
        cout << "Titular: " << nomeCliente[posicao] << endl;
                    
        bool encontrouMovimentacao = false;
                    
        // Passo por todo o histórico procurando o que bate com essa conta
        for (int i = 0; i < quantidadeMovimentacoes; i++) {
        if (contaMovimentacao[i] == numeroConta[posicao]) {
        encontrouMovimentacao = true;
        if (tipoMovimentacao[i] == 'D') {
            cout << "Deposito: R$ " << valorMovimentacao[i] << endl;
        } else if (tipoMovimentacao[i] == 'S') {
            cout << "Saque: R$ " << valorMovimentacao[i] << endl;
                }
            }
        }
                    
        if (!encontrouMovimentacao) {
            cout << "Nenhuma movimentacao registrada para esta conta." << endl;
        }
                    
        cout << "Saldo atual: R$ " << saldo[posicao] << endl;
        cout << "===========================================" << endl;
            }
        }
        break;
            
    // Opção 8: Altero o tipo da conta (de corrente para poupança ou vice-versa)
    case 8:
        {
        if (quantidadeContas == 0) {
            cout << "Nenhuma conta cadastrada." << endl;
        break;
        }
                
        int numeroBusca;
        int posicao = -1;
        int novoTipo;
        
        cout << "Digite o numero da conta: ";
        cin >> numeroBusca;
                
        for (int i = 0; i < quantidadeContas; i++) {
            if (numeroConta[i] == numeroBusca) {
        posicao = i;
        break;
            }
        }
                
        if (posicao == -1) {
            cout << "Conta nao encontrada." << endl;
        } else {
            cout << "Digite o novo tipo da conta (1 = Corrente, 2 = Poupanca): ";
        cin >> novoTipo;
                    
        if (novoTipo == 1 || novoTipo == 2) {
        tipoConta[posicao] = novoTipo;
        // Salvo no arquivo o novo tipo da conta
        salvarContas(numeroConta, nomeCliente, cpf, tipoConta, saldo, contaAtiva, quantidadeContas);
            cout << "Tipo da conta alterado com sucesso." << endl;
        } else {
            cout << "Tipo de conta inexistente." << endl;
                }
            }
        }
        break;
            
        // Opção 9: Ativo ou desativo a conta invertendo o booleano
    case 9:
        {
        if (quantidadeContas == 0) {
            cout << "Nenhuma conta cadastrada." << endl;
        break;
        }
                
        int numeroBusca;
        int posicao = -1;
                
        cout << "Digite o numero da conta: ";
        cin >> numeroBusca;
                
        for (int i = 0; i < quantidadeContas; i++) {
            if (numeroConta[i] == numeroBusca) {
            posicao = i;
        break;
            }
        }
                
        if (posicao == -1) {
            cout << "Conta nao encontrada." << endl;
        } else {
            if (contaAtiva[posicao] == true) {
            contaAtiva[posicao] = false;
            cout << "Conta desativada com sucesso." << endl;
        } else {
            contaAtiva[posicao] = true;
            cout << "Conta ativada com sucesso." << endl;
                }
            // Gravo no arquivo a mudança de ativa pra desativada ou vice-versa
            salvarContas(numeroConta, nomeCliente, cpf, tipoConta, saldo, contaAtiva, quantidadeContas);
            }
        }
        break;
            
    // Opção 10: Encerro o loop e saio do programa
    case 10:
        cout << "SAINDO DO SISTEMA..." << endl;
        break;
            
        default:
            cout << "OPCAO INVALIDA." << endl;
            break;
    }
    
} while (opcao != 10);

return 0;

}