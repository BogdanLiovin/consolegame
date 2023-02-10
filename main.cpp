#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <algorithm>
#include <math.h>
//#include "fights.cpp"

using namespace std;

// Класс бойца
class Fighter{
private:
  int damage; // Урон
  int enemiesDamaged; // Количество атакуемых
  int hpMax; // Максимальное здоровье
  int hp; // Здоровье

  Fighter* currentEnemy; // Текущая цель

  int stunDuration; // Количество секунд, на которые может оглушить противника
  int stunRechargeTimer; // Частота применения оглушения
  int stunRechargeLeft; // Оставшееся количество секунд до возможности оглушить врага
  int stunnedLeftSeconds; // Количество секунд оставшихя в оглушенном состоянии

public:
  // Конструктор без параметров создает персонажа со случайными характеристиками
  Fighter(){
    this->damage = 1 + (rand() % 5);
    this->enemiesDamaged = 1 + (rand() % 3);
    this->hpMax = 50 + (rand() % 200);
    this->stunDuration = 1 + (rand() % 5);
    this->stunRechargeTimer = 10 + (rand() % 11);
    this->reset();
  }

  // Конструктор с параметрами
  Fighter(int damage, int enemiesDamaged, int hpMax, int outAttackSeconds, int outAttackRechargeTimer){
    this->damage = damage;
    this->enemiesDamaged = enemiesDamaged;
    this->hpMax = hpMax;
    this->stunDuration = outAttackSeconds;
    this->stunRechargeTimer = outAttackRechargeTimer;
    this->reset();
  }

  // Геттеры и сеттеры
  int getDamage(){ return this->damage; }

  Fighter* getEnemy() { return this->currentEnemy; }

  int getEnemiesDamaged() { return this->enemiesDamaged; }

  int getHp() {return this->hp; }

  int getHpMax() {return this->hpMax; }

  int getStunDuration() { return this->stunDuration; }

  int getStunRechargeTimer() { return this->stunRechargeTimer; }

  void setEnemy(Fighter* enemy) { this->currentEnemy = enemy; }

  // Проверка выбывания
  bool isOut(){ return this->hp <= 0; }

  // Восстановить персонажа (необходимо после боя)
  void reset(){
    this->hp = this->hpMax;
    this->stunRechargeLeft = this->stunRechargeTimer;
    this->stunnedLeftSeconds = 0;
  }

  // Проверка на активность (статус оглушения и выбытие)
  bool isActive(){ return !(this->stunnedLeftSeconds > 0 || this->isOut()); }

  // Обработка получения урона
  void processHit(int hitDamage){
    //cout << "Урон получен\n";
    this->hp -= hitDamage;
  }

  // Обработка вывода из строя противником
  void processStun(int duration){
    //cout << "Оглушение наложено\n";
    this->stunnedLeftSeconds = duration + 1; // Поправка +1 делается в силу обработки прохождения секунды
  }

  // Обработка атаки текущей цели
  void attack(){
    //cout << "Атака\n";
    if (!this->isActive()) { return; }
    if (this->stunRechargeLeft <= 0){ 
      //cout << "Оглушение\n";
      this->currentEnemy->processStun(this->stunDuration);
      this->stunRechargeLeft = this->stunRechargeTimer;
      }
    this->currentEnemy->processHit(this->damage);
  }

  // Атака с параметром атакуемого врага (для командных боев)
  void attack(Fighter* enemy){
    //cout << "Атака с параметром\n";
    // Оглушает только активную цель
    if (!this->isActive()) { return; }
    if (this->stunRechargeLeft <= 0 && enemy->isActive()){ 
      //cout << "Оглушение\n";
      enemy->processStun(this->stunDuration);
      this->stunRechargeLeft = this->stunRechargeTimer;
    }
    enemy->processHit(this->damage);
  }

  // Обработка прохождения секунды
  void secondPassed(){
    if (this->stunnedLeftSeconds > 0) { this->stunnedLeftSeconds--; }
    if (!this->isActive()) { return; }
    if (this->stunRechargeLeft > 0) { this->stunRechargeLeft--; }
  }

  // Вывод статистики в консоль
  void printStats(){
    cout << "Урон - " << this->damage << endl;
    cout << "Здоровье - " << this->hp << "/" << this->hpMax << endl;
    cout << "Может раз в " << this->stunRechargeTimer << " секунд оглушить противника на " << this->stunDuration << " секунд" << endl;
  }

  // Вывод текущей статистики в консоль, более полный
  void printCurrent(){
    cout << "Активен - " << this->isActive() << endl;
    cout << "Урон - " << this->damage << endl;
    cout << "HP - " << this->hp << "/" << this->hpMax << endl;
    //cout << "Длительность оглушения - " << this->stunDuration << endl;
    //cout << "Восстановление оглушения - " << this->stunRechargeTimer << endl;
    cout << "До следующего оглушения - " << this->stunRechargeLeft << endl;
    cout << "Осталось быть оглушенным - " << this->stunnedLeftSeconds << endl;
    cout << endl;
  }

  bool equals(Fighter other){
    return (this->getDamage() == other.getDamage() &&
      this->getEnemiesDamaged() == other.getEnemiesDamaged() &&
      this->getHpMax() == other.getHpMax() &&
      this->getStunDuration() == other.getStunDuration() &&
      this->getStunRechargeTimer() == other.getStunRechargeTimer()
      );
  }
};

// Класс команды
class Team{
public:
  Fighter fighters[3]; // Массив бойцов от самого сильного к самому слабому

  Team* enemyTeam;

  Team(){ }

  // Конструктор команды, сразу сортирует по силе (по урону)
  Team(Fighter f1, Fighter f2, Fighter f3){
    this->fighters[0] = f1;
    this->fighters[1] = f2;
    this->fighters[2] = f3;
    Fighter buf;
    for (int i = 0; i < 3; i++){
      for (int j = i+1; j < 3; j++){
        if (this->fighters[i].getDamage() < this->fighters[j].getDamage()){
          buf = this->fighters[i];
          this->fighters[i] = this->fighters[j];
          this->fighters[j] = buf;
        }
      }
    }
  }

  // Установка основной цели
  void setEnemy(Team* enemyTeam){
    this->enemyTeam = enemyTeam;
  }

  // Вывод бойца
  Fighter getFighter(int position){
    return this->fighters[position];
  }

  bool isOut(){
    return (this->getFighter(0).isOut() && this->getFighter(1).isOut() && this->getFighter(2).isOut());
  }

  // Обработка атаки команды
  void attack(){
    // Обработка атаки каждого бойца
    for (int fighterNum = 0; fighterNum < 3; fighterNum++){
      //cout << "Боец №" << fighterNum+1 << endl;
      if (!this->getFighter(fighterNum).isActive()) {
        //cout << "Боец оглушен и не может атаковать" << endl;
        continue;
      }
      int attacksLeft = this->getFighter(fighterNum).getEnemiesDamaged();
      //cout << "Он может атаковать " << attacksLeft << " раз(а)" << endl;
      // Боец бьет всех кто еще жив в порядке от самого сильного к самому слабому
      for (int enemyNum = 0; enemyNum < 3; enemyNum++){
        // Если не осталось атак - закончить с этим бойцом
        if (attacksLeft <= 0) { break; }
        // Атакует только живых врагов
        if (!this->enemyTeam->fighters[enemyNum].isOut()){
          //cout << fighterNum+1 << " атакует " << enemyNum+1 << endl;
          this->fighters[fighterNum].attack(&this->enemyTeam->fighters[enemyNum]);
          attacksLeft--;
          //cout << "У бойца осталось еще " << attacksLeft << " атак" << endl;
        }
      }
    }
    //cout << endl;
  }

  // Обработка прохождения секунды
  void secondPassed(){
    for (int fighterNum = 0; fighterNum < 3; fighterNum++){
      this->fighters[fighterNum].secondPassed();
    }
  }

  // Вывод текущего состояния в консоль
  void printCurrent(){
    cout<<"Состояние команды:"<<endl;
    for (int fighterNum = 0; fighterNum < 3; fighterNum++){
      this->fighters[fighterNum].printCurrent();
    }
  }

  // Восстановление (необходимо после боя)
  void reset(){
    for (int fighterNum = 0; fighterNum < 3; fighterNum++){
      this->fighters[fighterNum].reset();
    }
  }

  bool equals(Team other){
    return (this->getFighter(0).equals(other.getFighter(0)) &&
      this->getFighter(1).equals(other.getFighter(1)) &&
      this->getFighter(2).equals(other.getFighter(2)));
  }
};

// Результаты боя 1 на 1
struct fightResults {
  int duration; // Длительность боя
  int winnerHpLeft; // Процент оставшегося здоровья у победителя
  int winner; // Победитель (1 или 2 переданный в функцию боя боец)
};

// Результаты боя команда на команду
struct teamFightResults {
  int duration; // Длительность боя
  int winners; // Победитель (1 или 2 переданная в функцию боя команда)
};

class Fight{
public:
  // Бой 1 на 1
  static fightResults Fight1vs1(Fighter f1, Fighter f2){
    int fightDuration = 0;
    f1.setEnemy(&f2);
    f2.setEnemy(&f1);
    while (true){
      fightDuration++;
      //cout << "***СЕКУНДА №" << fightDuration << "***\n";
      //cout << "Атака первого бойца" << endl;
      f1.attack();
      f1.secondPassed();
      //cout << "Атака второго бойца" << endl;
      f2.attack();
      f2.secondPassed();
      /*cout << "Состояние первого бойца:" << endl;
      f1.printCurrent();
      cout << "Состояние второго бойца:" << endl;
      f2.printCurrent();*/
      if (f1.isOut() || f2.isOut()){
        break;
      }
    }
    //f1.printCurrent();
    //f2.printCurrent();
    fightResults results;
    results.duration = fightDuration;
    if (f1.isOut()) {
      results.winnerHpLeft = (100 * f2.getHp())/f2.getHpMax();
      results.winner = 2;
    }
    else {
      results.winnerHpLeft = (100 * f1.getHp())/f1.getHpMax();
      results.winner = 1;
    }
    f1.reset();
    f2.reset();
    return results;
  }

  // Бой команда на команду
  static teamFightResults FightTeamVsTeam(Team t1, Team t2){
    int fightDuration = 0;
    //t1.printCurrent();
    //t2.printCurrent();
    t1.setEnemy(&t2);
    t2.setEnemy(&t1);
    while (true){
      fightDuration++;
      //cout << "***СЕКУНДА №" << fightDuration << "***\n";
      //cout << "Атаки команды 1" << endl;
      t1.attack();
      t1.secondPassed();
      //cout << "Атаки команды 2" << endl;
      t2.attack();
      t2.secondPassed();
      //t1.printCurrent();
      //t2.printCurrent();
      if (t1.isOut() || t2.isOut()) { break; }
    }
    teamFightResults results;
    results.duration = fightDuration;
    if (t1.isOut()) { results.winners = 2; }
    else { results.winners = 1; }
    t1.reset(); t2.reset();
    return results;
  }
};

// Факториал для вычисления количества команд
int fact(int n){
  if (n==0){ return 1; }
  else { return n * fact(n-1); };
}

// Вычисление количества строк в файле (количества бойцов)
int getLines(){
  int counter = 0;
  ifstream file ("fighters.txt");
  string s;
  if (file.is_open()){
    while (getline(file,s)){
      counter++;
    }
    file.close();
  }
  return counter;
}

// Считывание бойцов из текстового файла
void readFightersFromFile(Fighter array[]){
  int counter = 0;
  string line;
  string token;
  int stats[5];
  ifstream file ("fighters.txt");
  size_t pos = 0;
  if (file.is_open()){
    while (getline(file,line)){
      line = line + ' ';
      //cout << "Line: " << line << endl;
      for (int i=0; i<5; i++) {
        pos = line.find(' ');
        token = line.substr(0, pos);
        //cout << token << endl;
        stats[i] = stoi(token);
        line.erase(0, pos + 1);
      }
      array[counter] = Fighter(stats[0], stats[1], stats[2], stats[3], stats[4]);
      counter++;
    }
    file.close();
  }
}

int main() {
  int allFightersLen = getLines();
  Fighter allFighters[allFightersLen];
  readFightersFromFile(allFighters);
  for (int i=0; i<allFightersLen; i++){
    cout << "Боец №" << i << ":\n";
    allFighters[i].printStats();
    cout<<endl;
  }
  int enemyFighterNum;
  string sortingType;

  // ЗАДАЧА А
  // Ввод данных
  cout << "Введите номер бойца противника: ";
  cin >> enemyFighterNum;
  cout << "Введите вид сортировки победителей" << endl;
  cout << "\"hp\" - по остатку здоровья" << endl;
  cout << "\"dur\" - по длительности боя" << endl;
  cout << "Выбор: ";
  cin >> sortingType;

  // Проведение боев и формирование результатов
  Fighter enemyFighter = allFighters[enemyFighterNum];
  fightResults fr1v1;
  int allFightResults[allFightersLen][2];
  allFightResults[enemyFighterNum][1] = -1;
  for (int i=0; i<allFightersLen; i++){
    allFightResults[i][0] = i;
    if (i == enemyFighterNum){ continue; }
    //cout << "Против бойца №" << i << endl;
    fr1v1 = Fight::Fight1vs1(enemyFighter, allFighters[i]);
    if (fr1v1.winner == 2){
      //cout << "Боец победил врага за " << fr1v1.duration << " секунд. При этом у него осталось " << fr1v1.winnerHpLeft << "% здоровья" << endl;
      //allFighters[i].printStats();
      if (sortingType == "hp"){
        allFightResults[i][1] = fr1v1.winnerHpLeft;
      }
      else if (sortingType == "dur"){
        allFightResults[i][1] = fr1v1.duration;
      }
    }
    else{
      //cout << "Боец №" << i << " проиграл бойцу врага " << endl;
      allFightResults[i][1] = -1;
    }
    //cout << endl;
  }

  // Сортировка массива результатов
  for (int i=0; i<allFightersLen; i++){
    for (int j=i+1; j<allFightersLen; j++){
      if (sortingType == "hp"){
        if (allFightResults[i][1] < allFightResults[j][1]){
          int temp0 = allFightResults[i][0];
          int temp1 = allFightResults[i][1];
          allFightResults[i][0] = allFightResults[j][0];
          allFightResults[i][1] = allFightResults[j][1];
          allFightResults[j][0] = temp0;
          allFightResults[j][1] = temp1;
        }
      }
      else if (sortingType == "dur"){
        if (allFightResults[i][1] > allFightResults[j][1]){
          int temp0 = allFightResults[i][0];
          int temp1 = allFightResults[i][1];
          allFightResults[i][0] = allFightResults[j][0];
          allFightResults[i][1] = allFightResults[j][1];
          allFightResults[j][0] = temp0;
          allFightResults[j][1] = temp1;
        }
      }
    }
  }

  // Проверочный вывод итоговых результатов
  /*for (int i=0; i<allFightersLen; i++){
    cout << allFightResults[i][0] << " - " << allFightResults[i][1] << endl;
  }*/

  // Вывод массива результатов
  cout << "Победу над бойцом №" << enemyFighterNum << " одержали следующие бойцы:" << endl;
  for (int i=0; i<allFightersLen; i++){
    if (allFightResults[i][1] != -1){
      cout << "Боец №" << allFightResults[i][0] << " победил противника ";
      if (sortingType == "hp"){
        cout << "и остался с " << allFightResults[i][1] << "% здоровья" << endl;
      }
      else if (sortingType == "dur"){
        cout << "за " << allFightResults[i][1] << " секунд" << endl;
      }
    }
  }
  cout << endl;

  // ЗАДАЧА В
  // Ввод данных
  int enemyTeam1, enemyTeam2, enemyTeam3;
  cout << "Введите номер первого бойца в команде: ";
  cin >> enemyTeam1;
  cout << "Введите номер второго бойца в команде: ";
  cin >> enemyTeam2;
  cout << "Введите номер третьего бойца в команде: ";
  cin >> enemyTeam3;
  int enemyNums[4] = {enemyTeam1, enemyTeam2, enemyTeam3, -1};

  Team enemyTeam = Team(allFighters[enemyTeam1], allFighters[enemyTeam2], allFighters[enemyTeam3]);

  // В каждом массиве содержатся номера бойцов в команде и длительность боя
  int possibleTeams = fact(allFightersLen-3)/(fact(3)*fact(allFightersLen-6));
  int allTeamFightResults[possibleTeams][4];

  // Проведение боев и формирование результатов
  int teamCounter = 0;
  Team team;
  for (int i=0; i<allFightersLen; i++){
    for (int j=i+1; j<allFightersLen; j++){
      for (int k=j+1; k<allFightersLen; k++){

        // Проверка на нахождение в команде взятого противником бойца
        if ((find(begin(enemyNums), end(enemyNums), i) != end(enemyNums)) | (find(begin(enemyNums), end(enemyNums), j) != end(enemyNums)) | (find(begin(enemyNums), end(enemyNums), k) != end(enemyNums))){
          continue; 
        }
        else{
          allTeamFightResults[teamCounter][0] = i;
          allTeamFightResults[teamCounter][1] = j;
          allTeamFightResults[teamCounter][2] = k;
          team = Team(allFighters[i], allFighters[j], allFighters[k]);
          teamFightResults tfr = Fight::FightTeamVsTeam(enemyTeam, team);
          if (tfr.winners == 1){ allTeamFightResults[teamCounter][3] = -1; }
          else { allTeamFightResults[teamCounter][3] = tfr.duration; }
          teamCounter++;
        }
      }
    }
  }

  // Сортировка массива результатов
  int temp[4];
  for (int i=0; i<possibleTeams; i++){
    for (int j=i+1; j<possibleTeams; j++){
      if (allTeamFightResults[i][3] > allTeamFightResults[j][3]){
        for (int k=0; k<4; k++){
          temp[k] = allTeamFightResults[i][k];
          allTeamFightResults[i][k] = allTeamFightResults[j][k];
          allTeamFightResults[j][k] = temp[k];
        }
      }
    }
  }

  // Вывод данных
  for (int i=0; i<possibleTeams; i++){
    if (allTeamFightResults[i][3] != -1){
      cout << "Команда из бойцов №" << allTeamFightResults[i][0] << ", №" << allTeamFightResults[i][1] << ", №" << allTeamFightResults[i][2] << " победила вражескую команду за " << allTeamFightResults[i][3] << " секунд" << endl;
    }
  }
}