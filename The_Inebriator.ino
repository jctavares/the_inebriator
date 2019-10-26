//=========================== The Inebriator =========================//
// Alexandre "Xande" Lakiss
// Gleice "Gle Ice" Kelly
// João "Lucas" Coelho
// José "Zuzé" Rodolfo

//=========================== BIBLIOTECAS =========================//
#include <LiquidCrystal.h> // Referente ao display

//=========================== DEFINIÇÕES =========================//
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // Define os pinos que serão utilizados para ligação ao display
#define Rele_Peltier 31
#define Rele_Solenoide 29
#define Rele_Bomba 27

//=========================== Declarando Variáveis Globais =========================//
int ValorAjustadoTemp = 25; // Variável de seleção de temperatura desejada, que inicia em 25º
int valor_menu = 0; // Valor utilizado para navegar pelo menu
int pos = 5; // Posição atribuída para confirmação e atualização do display
const int LM35 = A0; // Define o pino que lera a saída do LM35
float temperatura; // Variável que armazenará a temperatura medida

// Verificação dos estados dos botões
int menu_mais = LOW;
int menu_menos = LOW;
int menu_confirma = LOW;
int temp_mais = LOW;
int temp_menos = LOW;

// Declaração das portas dos botões
int botao_mais = 13;
int botao_menos = 10;
int botao_selecionar = 9;
int botao_temp_mais = 23;
int botao_temp_menos = 25;



void  setup() {
  // Configuração das portais digitais dos relês
  pinMode(Rele_Peltier, OUTPUT);
  pinMode(Rele_Solenoide, OUTPUT);
  pinMode(Rele_Bomba, OUTPUT);

  lcd.begin(16, 2); // Inicia do LCD (colunas x linhas)   

  // Configuração dos botões
  pinMode(botao_mais, INPUT);
  pinMode(botao_menos, INPUT);
  pinMode(botao_selecionar, INPUT);
  pinMode(botao_temp_mais, INPUT);
  pinMode(botao_temp_menos, INPUT);
}

void loop()
{ 

  lcd.setCursor(0, 0);
  lcd.print("Cana: ");
  lcd.setCursor(13, 0);
  
  // Garante que o valor possui duas casas adicionando um zero na frente caso ele seja menor que 10
  if (ValorAjustadoTemp < 10) {
    lcd.print("0");
    lcd.setCursor(14, 0);
    lcd.print(ValorAjustadoTemp);
  } else {
    lcd.print(ValorAjustadoTemp);
  }
  lcd.print("C");
  lcd.setCursor(13, 1);

  // Imprime a temperatura atual baseada no sensor
  temperatura = mediaTemp();
  lcd.print(temperatura);
  
  // Condição que permite ativação do botão com um único clique
  if ((debounceButton(temp_mais, botao_temp_mais) == HIGH && ValorAjustadoTemp < 30) && temp_mais == LOW) { // Aumenta a temperatura desejada
    ValorAjustadoTemp++;
    temp_mais = HIGH;
  }else if(debounceButton(temp_mais, botao_temp_mais) == LOW && temp_mais == HIGH){
    temp_mais = LOW;
  }
  if ((debounceButton(temp_menos, botao_temp_menos) == HIGH && ValorAjustadoTemp > 0) && temp_menos == LOW){ // Reduz a temperatura desejada
    ValorAjustadoTemp--;
    temp_menos = HIGH;
  }else if(debounceButton(temp_menos, botao_temp_menos) == LOW && temp_menos == HIGH){
    temp_menos = LOW;
  }

  if (debounceButton(menu_mais, botao_mais) == HIGH && menu_mais == LOW) { // Avança o menu
    valor_menu++;
    menu_mais = HIGH;
  }else if(debounceButton(menu_mais, botao_mais) == LOW && menu_mais == HIGH){
    menu_mais = LOW;
  }
  
  if (debounceButton(menu_menos, botao_menos) == HIGH && menu_menos == LOW) { // Volta o menu
    valor_menu += 2;
    menu_menos = HIGH;
  }else if(debounceButton(menu_menos, botao_menos) == LOW && menu_menos == HIGH){
    menu_menos = LOW;
  }

  valor_menu = valor_menu % 3; // Mantém o valor da variável entre 0 e 2 (3 opções de menu)
  if(pos != valor_menu){ // Condição de atualização do menu no display, para evitar que fique piscando
    switch (valor_menu)
    {
      case 0:
        lcd.clear();
        lcd.setCursor(0, 4);
        lcd.print(">Vodka");
        break;
  
      case 1:
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print(">Suco");
        break;
  
      case 2:
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print(">Suco Batiz.");
        break;
    }
  }
  
  pos = valor_menu;
  if (debounceButton(menu_confirma, botao_selecionar) == HIGH && menu_confirma == LOW) { // Condição de clique do botão "confirma"
    confirma();
    menu_confirma == HIGH;
  }else if(debounceButton(menu_confirma, botao_selecionar) == LOW && menu_confirma == HIGH){
    menu_confirma == LOW;
  }

}

void drink_pronto() // Ativa a válvula solenoide para despejar o líquido no copo
{
  lcd.clear();
  lcd.print("Servindo...");
  digitalWrite(Rele_Solenoide, HIGH);
  delay(5000);
  digitalWrite(Rele_Solenoide, LOW);
}
void ativa_bomba() // Bombeia o líquido no funil
{
  lcd.clear();
  lcd.print("Bombeando...");
  digitalWrite(Rele_Bomba, HIGH);
  delay(5000);
  digitalWrite(Rele_Bomba, LOW);
}

void gela_bebida() // Ativa a peltier e gela o líquido dentro do funil de alumínio
{
  lcd.clear();
  lcd.print("Gelando...");
  unsigned long tempo = millis(); // Guarda o tempo do millis ao iniciar a função
  temperatura = mediaTemp(); // Temperatura do sensor
  
  if (temperatura >= ValorAjustadoTemp ) { // Ativa a peltier caso a temperatura atual < desejada
    digitalWrite(Rele_Peltier, HIGH);
  }
  do {  
    temperatura = mediaTemp();
    // Verifica a temperatura até a temp. desejada ser atingida ou alcaçar o tempo limite (10 segundos para testes)
  } while (temperatura > ValorAjustadoTemp && millis() < (tempo + 10000));
  digitalWrite(Rele_Peltier, LOW);
  }

void confirma() // Função para chamar os procedimentos da bebida escolhida
{
  lcd.clear();
  lcd.print("Se garante!");
  delay(1500);
  lcd.clear();
  lcd.print("Aguarde sua");
  lcd.setCursor(0, 1);
  lcd.print("bebida! :)");
  delay(1000);

  // Para o primeiro GQ só tivemos uma opção configurada (bebida única)
  switch (pos)
  {
    case 0:
    ativa_bomba();
    gela_bebida();
    drink_pronto();
    break;
    case 1:
    //Fica para a próxima torre
    break;
    case 2:
    //etc
    break;
 }
  lcd.clear();
  
  // Reseta as variáveis
  valor_menu = 0;
  pos = 5;
}

float mediaTemp() // Faz uma média (de 1000 medidas) da temperatura com base na informação do sensor
 {
  int i;
  float resultado = 0;
  for(i = 0; i < 1000; i++){
      resultado += (float(analogRead(LM35))*5/(1023))/0.01; // Lê e converte a medida do sensor para ºC
    }
    return resultado / 1000;
 }
 
boolean debounceButton(boolean estado, int botao) // Função que impede o clique repetido do botão ao pressionar apenas uma vez
{
  boolean estadoAtual = digitalRead(botao);
  if(estado != estadoAtual)
  {
    delay(10);
    estadoAtual = digitalRead(botao);
  }
  return estadoAtual;
}
