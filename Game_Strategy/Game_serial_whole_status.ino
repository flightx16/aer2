#define NUM_COL 7
#define NUM_ROW 6
#define NONE 0
#define Lala 1
#define Tabuki 2
#define SS 3; //Sensitivity Spot
#define FS 4; //Forbidden Spot
#define WS 5; //Warning Spot
#define TRUE 1;
typedef struct {
	int Player; // 1-Lala, 2-Tabuki, 0-None
	int Sensitivity;//Sensitivity zone
	int Forbidden;//Forbidden zone
	int Warning;//Warning zone
	int V4;//the spot is already in a vertical connecting four sequence
	int H4;//the spot is already in a horizontal connecting four sequence
	int D41;//the spot is already in a diagonal type 1 connecting four sequence
	int D42;//the spot is already in a diagonal type 2 connecting four sequence
} Piece;

Piece Board[NUM_COL][NUM_ROW];



void initialize (){
	for (int i = 0; i < NUM_COL; i++){
		for (int j = 0; j < NUM_ROW; j++){
			Board[i][j].Player = NONE;
			Board[i][j].Sensitivity = NONE;
			Board[i][j].Forbidden = NONE;
			Board[i][j].Warning = NONE;
			Board[i][j].V4 = NONE;
			Board[i][j].H4 = NONE;
			Board[i][j].D41 = NONE;
			Board[i][j].D42 = NONE;
		}
	}
}

void reset(){
	for (int i = 0; i < NUM_COL; i++){
		for (int j = 0; j < NUM_ROW; j++){
			Board[i][j].Player = NONE;
			Board[i][j].Sensitivity = NONE;
			Board[i][j].Forbidden = NONE;
			Board[i][j].Warning = NONE;
			Board[i][j].V4 = NONE;
			Board[i][j].H4 = NONE;
			Board[i][j].D41 = NONE;
			Board[i][j].D42 = NONE;
		}
	}
}



//The following function returns the number of row which the piece will be availble to drop
int check_top_row(int col_num){
	for (int j = 0; j < NUM_ROW; j ++){
		if (Board[col_num][j].Player == NONE) {return j;}
	}
	return NUM_ROW;
}

void droppiece(int player, int col_num){
	if (check_top_row(col_num) < NUM_ROW) Board[col_num][check_top_row(col_num)].Player = player;
}

bool respond(){
	int bus = digitalRead(3);
	if (bus == HIGH) {
		while (bus == HIGH){
			delay(10);
			bus = digitalRead(3);
		}
		return true;
	}
	return false;
}

void serial_print_gamestatus(){
      Serial.print("Sf");
	for (int i = 0; i < NUM_COL; i ++){
        for(int j = 0; j < NUM_ROW; j++){
        	if (Board[i][j].Player !=NONE) Serial.print(Board[i][j].Player);
        	else if (Board[i][j].Forbidden != NONE) Serial.print(Board[i][j].Forbidden);
        	else Serial.print(NONE);
	  	}
        Serial.print("f");
    }
    Serial.println("E");
} 

 int check_button_pressed(){
	for( int i=0;i<=5;i++){
	int bus=analogRead(i);
        //Serial.println(bus);
	if(bus>=500) {
		while(bus>500){
		delay(10);
		bus=analogRead(i);
		}
		return (6-i);
	}
	}
        delay(10);
	int bus=digitalRead(2);
	if(bus== HIGH) {
		while(bus==HIGH){
		delay(10);
		bus=digitalRead(2);
		}
		return 7;
	}
	return 0;
}
	
void setup(){
	pinMode(7, OUTPUT);
	pinMode(8, OUTPUT);
	pinMode(9, OUTPUT);
	pinMode(10, OUTPUT);
	pinMode(11, OUTPUT);
	pinMode(12, OUTPUT);
	pinMode(13, OUTPUT);
	initialize();
	Serial.begin(9600);
    serial_print_gamestatus();
}

void loop(){
	int button_pressed = check_button_pressed();
  	if (button_pressed != 0) {droppiece(Tabuki, button_pressed - 1); serial_print_gamestatus();}
  	if (respond() == true) {droppiece(Lala, strategy()); serial_print_gamestatus();}
  	delay(100);
};

int strategy(){
	refresh();
	identify_connecting_four();
	identify_sensitivity_forbidden();
	identify_warning();
	int emergency_block_col_num;
	emergency_block_col_num = block_vertical_three();
	if (emergency_block_col_num >= 0 && emergency_block_col_num < NUM_COL) return emergency_block_col_num;
	emergency_block_col_num = block_diagonal_three();
	if (emergency_block_col_num >= 0 && emergency_block_col_num < NUM_COL) return emergency_block_col_num;
	emergency_block_col_num = block_horizontal_three();
	if (emergency_block_col_num >= 0 && emergency_block_col_num < NUM_COL) return emergency_block_col_num;
	emergency_block_col_num = trap_1();
	if (emergency_block_col_num >= 0 && emergency_block_col_num < NUM_COL) return emergency_block_col_num;
	emergency_block_col_num = trap_2();
	if (emergency_block_col_num >= 0 && emergency_block_col_num < NUM_COL) return emergency_block_col_num;
	if (Board[3][4].Player == NONE) return 3;
	emergency_block_col_num = drop_warning();
	if (emergency_block_col_num >= 0 && emergency_block_col_num < NUM_COL) return emergency_block_col_num;
	

	

	return 1;
}

void refresh(){
	for (int i = 0; i < NUM_COL; i++){
		for (int j = 0; j < NUM_ROW; j++){
			Board[i][j].Sensitivity = NONE;
			Board[i][j].Forbidden = NONE;
			Board[i][j].Warning = NONE;
		}
	}
}
/***************************************************************************************************************************************************/
void identify_connecting_four(){
	identify_connecting_four_hor();
	identify_connecting_four_ver();
	identify_connecting_four_dia1();
	identify_connecting_four_dia2();
}

void identify_connecting_four_hor(){
	for (int i = 0; i < 4; i ++){
		for (int j = 0; j < NUM_ROW; j ++){
			if (Board[i][j].Player == Tabuki && Board[i+1][j].Player == Tabuki && Board[i+2][j].Player == Tabuki && Board[i+3][j].Player == Tabuki){
				Board[i][j].H4 = 1; Board[i+1][j].H4 = 1; Board[i+2][j].H4 = 1; Board[i+3][j].H4 = 1;
			}
		}
	}
}

void identify_connecting_four_ver(){
	for (int i = 0; i < NUM_COL; i ++){
		for (int j = 0; j <3; j ++){
			if (Board[i][j].Player == Tabuki && Board[i][j+1].Player == Tabuki && Board[i][j+2].Player == Tabuki && Board[i][j+3].Player == Tabuki){
				Board[i][j].V4 = 1; Board[i][j+1].V4 = 1; Board[i][j+2].V4 = 1; Board[i][j+3].V4 = 1;
			}
		}
	}

}

void identify_connecting_four_dia1(){
	for (int i = 0; i < 4; i ++){
		for (int j = 0; j < 3; j ++){
			if (Board[i][j].Player == Tabuki && Board[i+1][j+1].Player == Tabuki && Board[i+2][j+2].Player == Tabuki && Board[i+3][j+3].Player == Tabuki){
				Board[i][j].D41 == 1 ; Board[i+1][j+1].D41 == 1 ; Board[i+2][j+2].D41 == 1 ; Board[i+3][j+3].D41 == 1;
			}
		}
	}

}

void identify_connecting_four_dia2(){
	for (int i = 3; i < NUM_COL; i ++){
		for (int j = 0; j < 3; j ++){
			if (Board[i][j].Player == Tabuki && Board[i-1][j+1].Player == Tabuki && Board[i-2][j+2].Player == Tabuki && Board[i-3][j+3].Player == Tabuki){
				Board[i][j].D42 == 1 ; Board[i-1][j+1].D42 == 1 ; Board[i-2][j+2].D42 == 1 ; Board[i-3][j+3].D42 == 1;
			}
		}
	}
}
/************************************/
void identify_warning(){
	identify_warning_hor();
	identify_warning_dia();
}
/***************************************************************************************************************************************************/
void identify_warning_hor(){
	for (int i = 0; i < 4; i ++){
		for (int j = 1; j < NUM_ROW; j++){
			markwarning_hor(i,j);
		}
	}
}
//mark forbidden zone for a connecting four sequence beginning at row_num, col_num
void markwarning_hor(int col_num, int row_num){
	int Tabuki_spot_num = 0;
	for(int i=col_num; i<col_num + 4; i++){
	    if (Board[i][row_num].Player == Lala || Board[i][row_num].H4 == 1) return ;
	    else if (Board[i][row_num].Player == Tabuki) Tabuki_spot_num++;
	}
	if (Tabuki_spot_num >=2 ){
		for(int i=col_num; i<col_num + 4; i++){
			if (Board[i][row_num].Player == NONE ) { Board[i][row_num].Warning = WS;}
		}
	}	
}
/***************************************************************************************************************************************************/
void identify_warning_dia(){
	for(int i = 0; i < 4; i ++){
		for(int j = 0; j <3 ;j ++){
			markwarning_dia1(i,j);
		}
	}
	for(int i = 3; i < NUM_COL; i++){
		for(int j = 0; j <3 ;j ++){
			markwarning_dia2(i,j);
		}	    
	}
}

void markwarning_dia1(int col_num, int row_num){
	char Tabuki_spot_num = 0;
	for(int i=0; i< 4; i++){
	    if (Board[col_num + i][row_num + i].Player == Lala|| Board[col_num + i][row_num + i].D41 == 1) return ;
	    else if (Board[col_num + i][row_num + i].Player == Tabuki) Tabuki_spot_num++;
	}
	if (Tabuki_spot_num >=2){
		for(int i=0; i< 4; i++){
			if (Board[col_num + i][row_num + i].Player == NONE) {
				Board[col_num + i][row_num + i].Warning = WS;
			}
		}
	}	
}

void markwarning_dia2(int col_num, int row_num){
	char Tabuki_spot_num = 0;
	for(int i=0; i< 4; i++){
	    if (Board[col_num - i][row_num + i].Player == Lala || Board[col_num - i][row_num + i].D42 == 1) return ;
	    else if (Board[col_num - i][row_num + i].Player == Tabuki) Tabuki_spot_num++;
	}
	if (Tabuki_spot_num >=2){
		for(int i=0; i< 4; i++){
			if (Board[col_num - i][row_num + i].Player == NONE ) {
				Board[col_num - i][row_num + i].Warning = WS;
			}
		}
	}	
}
/*****************HHHHHHHHHHHHHHH************/
/***************************************************************************************************************************************************/
void identify_sensitivity_forbidden(){
	identify_forbidden_hor();
	identify_forbidden_dia();
}
/***************************************************************************************************************************************************/
void identify_forbidden_hor(){
	for (int i = 0; i < 4; i ++){
		for (int j = 1; j < NUM_ROW; j++){
			markforbidden_hor(i,j);
		}
	}
}
//mark forbidden zone for a connecting four sequence beginning at row_num, col_num
void markforbidden_hor(int col_num, int row_num){
	int Tabuki_spot_num = 0;
	for(int i=col_num; i<col_num + 4; i++){
	    if (Board[i][row_num].Player == Lala || Board[i][row_num].H4 == 1) return ;
	    else if (Board[i][row_num].Player == Tabuki) Tabuki_spot_num++;
	}
	if (Tabuki_spot_num >=2 ){
		for(int i=col_num; i<col_num + 4; i++){
			if (Board[i][row_num].Player == NONE && Board[i][row_num - 1].Player == NONE) {Board[i][row_num - 1].Forbidden = FS;}
		}
	}	
}
/***************************************************************************************************************************************************/
void identify_forbidden_dia(){
	for(int i = 0; i < 4; i ++){
		for(int j = 0; j <3 ;j ++){
			markforbidden_dia1(i,j);
		}
	}
	for(int i = 3; i < NUM_COL; i++){
		for(int j = 0; j <3 ;j ++){
			markforbidden_dia2(i,j);
		}	    
	}
}

void markforbidden_dia1(int col_num, int row_num){
	char Tabuki_spot_num = 0;
	for(int i=0; i< 4; i++){
	    if (Board[col_num + i][row_num + i].Player == Lala|| Board[col_num + i][row_num + i].D41 == 1) return ;
	    else if (Board[col_num + i][row_num + i].Player == Tabuki) Tabuki_spot_num++;
	}
	if (Tabuki_spot_num >=2){
		for(int i=0; i< 4; i++){
			if (Board[col_num + i][row_num + i].Player == NONE && (row_num + i - 1) >= 0 && Board[col_num+ i][row_num + i - 1].Player == NONE) {
				Board[col_num + i][row_num + i - 1].Forbidden = FS;
			}
		}
	}	
}

void markforbidden_dia2(int col_num, int row_num){
	char Tabuki_spot_num = 0;
	for(int i=0; i< 4; i++){
	    if (Board[col_num - i][row_num + i].Player == Lala || Board[col_num - i][row_num + i].D42 == 1) return ;
	    else if (Board[col_num - i][row_num + i].Player == Tabuki) Tabuki_spot_num++;
	}
	if (Tabuki_spot_num >=2){
		for(int i=0; i< 4; i++){
			if (Board[col_num - i][row_num + i].Player == NONE && (row_num + i - 1) >= 0 && Board[col_num - i][row_num + i - 1].Player == NONE) {
				Board[col_num - i][row_num + i - 1].Forbidden = FS;
			}
		}
	}	
}
/***************************************************************************************************************************************************/
int block_vertical_three(){
	char num_block = 0;
	int blockv3_col_num;
	for (int i = 0; i < NUM_COL; i ++){
		if (Board[i][2].Player == Lala || Board[i][3].Player == Lala) continue;
		for (int j = 0; j < 3; j ++){
			if (Board[i][j].Player == Tabuki && Board[i][j+1].Player == Tabuki && Board[i][j+2].Player == Tabuki && Board[i][j+3].Player == NONE 
				&& Board[i][j].V4 == 0 && Board[i][j+1].V4 == 0 && Board[i][j+2].V4 == 0 && Board[i][j+3].V4 == 0) {num_block++; blockv3_col_num = i;}
		}
	}
	//if (num_block > 1) call selection function and then return from selection function
	//else if (num_block == 1) return block_col_num;
	if (num_block > 1) return blockv3_col_num;
	if (num_block == 1) return blockv3_col_num;
	return 8;
}

int block_diagonal_three(){
	char num_block = 0;
	int blockd3_col_num;
	int col_offset;
	for (int i = 0; i < 4; i ++){
		for (int j = 0; j <3; j ++){
			col_offset = check_diagonal_three1(i,j);
			if (col_offset != 4) {num_block++; blockd3_col_num = i + col_offset ;}
		}
	}
	for(int i = 3; i < NUM_COL; i ++){
		for (int j = 0; j < 3; j ++){
			col_offset = check_diagonal_three2(i,j);
			if (col_offset != 4) {num_block++; blockd3_col_num = i - col_offset ;}
		}
	}
	//if (num_block > 1) call selection function and then return from selection function
	//else if (num_block == 1) return blockd3_col_num;
	if (num_block > 1) return blockd3_col_num;
	if (num_block == 1) return blockd3_col_num;
	return 8;
}

int check_diagonal_three1(int col_num, int row_num){
	char Tabuki_spot_num = 0;
	for (int i = 0; i < 4; i ++){
		if (Board[col_num + i][row_num + i].Player == Lala || Board[col_num + i][row_num + i].D41 == 1) return 4;//Connecting four not detected for the specific sequence
		else if (Board[col_num + i][row_num + i].Player == Tabuki) Tabuki_spot_num++;
	}
	if (Tabuki_spot_num == 3){
		for (int i = 0; i < 4; i ++){
			if (Board[col_num + i][row_num + i].Player == NONE && ((row_num == 0 && i == 0) || Board[col_num + i][row_num + i -1].Player != NONE)) return i;
		}
	}
	return 4;
}

int check_diagonal_three2(int col_num, int row_num){
	char Tabuki_spot_num = 0;
	for (int i = 0; i < 4; i ++){
		if (Board[col_num - i][row_num + i].Player == Lala || Board[col_num - i][row_num + i].D42 == 1) return 4;//Connecting four not detected for the specific sequence
		else if (Board[col_num - i][row_num + i].Player == Tabuki) Tabuki_spot_num++;
	}
	if (Tabuki_spot_num == 3){
		for (int i = 0; i < 4; i ++){
			if (Board[col_num - i][row_num + i].Player == NONE && ((row_num == 0 && i == 0) || Board[col_num - i][row_num + i -1].Player != NONE)) return i;
		}
	}
	return 4;
}

int block_horizontal_three(){
	char num_block = 0;
	int blockh3_col_num;
	for (int j = 0; j <NUM_ROW; j++){
		if (Board[3][j].Player == Lala) continue;
		for (int i = 0; i < 4; i ++){
			char Tabuki_spot_num = 0;
			for (int k = 0; k < 4; k ++){
				if (Board[i+k][j].H4 == 1) break;
				if (Board[i+k][j].Player == Tabuki) Tabuki_spot_num++;
			}
			if (Tabuki_spot_num == 3){
				for(int k = 0; k < 4; k ++){
					if (Board[i+k][j].Player == NONE && (j == 0 || Board[i+k][j - 1].Player != NONE )) {num_block ++; blockh3_col_num = i+k;}
				}
			}
		}
	}
	if (num_block > 1) return blockh3_col_num;//cal  selection
	else if (num_block == 1) return blockh3_col_num;
	return 8;
}

int trap_1(){
	for (int j = 0; j < 1; j ++){
		if (Board[0][j].Player == NONE && Board[1][j].Player == NONE && Board[2][j].Player == Tabuki && Board[3][j].Player == Tabuki &&Board[4][j].Player == NONE &&Board[5][j].Player == NONE ) return 4;
		if (Board[1][j].Player == NONE && Board[2][j].Player == NONE && Board[3][j].Player == Tabuki && Board[4][j].Player == Tabuki &&Board[5][j].Player == NONE &&Board[6][j].Player == NONE ) return 2;
		return 8;
	}
}

int trap_2(){
	if (Board[1][0].Player == NONE && Board[2][0].Player == Tabuki && Board[3][0].Player == Tabuki && Board[4][0].Player == NONE && Board[5][0].Player == NONE) return 4;
	if (Board[1][0].Player == NONE && Board[2][0].Player == NONE && Board[3][0].Player == Tabuki && Board[4][0].Player == Tabuki && Board[5][0].Player == NONE) return 2;
	return 8;
}

int drop_warning(){
	char drop_warning_num = 0;
	int place_col_num;
	for (int i = 0; i < NUM_COL; i ++){
		if (check_top_row(i) < NUM_ROW && Board[i][check_top_row(i)].Warning == 5 ){
			drop_warning_num++; 
			place_col_num = i;
		}
	}
	if (drop_warning_num == 1) return place_col_num;
	if (drop_warning_num > 1) return place_col_num; //use selection to decide
	if (drop_warning_num == 0) return 8;
}
