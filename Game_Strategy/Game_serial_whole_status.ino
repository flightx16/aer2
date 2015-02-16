#define NUM_COL 7
#define NUM_ROW 6
#define NONE 0
#define Lala 1
#define Tabuki 2
#define SS 3; //Sensitivity Spot
#define FS 4; //Forbidden Spot
typedef struct {
	int Player; // 1-Lala, 2-Tabuki, 0-None
	int Sensitivity;//Sensitivity zone
	int Forbidden;//Forbidden zone
} Piece;

Piece Board[NUM_COL][NUM_ROW];



void initialize (){
	for (int i = 0; i < NUM_COL; i++){
		for (int j = 0; j < NUM_ROW; j++){
			Board[i][j].Player = NONE;
			Board[i][j].Sensitivity = NONE;
			Board[i][j].Forbidden = NONE;
		}
	}
}

void reset(){
	for (int i = 0; i < NUM_COL; i++){
		for (int j = 0; j < NUM_ROW; j++){
			Board[i][j].Player = NONE;
			Board[i][j].Sensitivity = NONE;
			Board[i][j].Forbidden = NONE;
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
	identify_sensitivity_forbidden();
	int emergency_block_col_num;
	emergency_block_col_num = block_vertical_three();
	if (emergency_block_col_num >= 0 && emergency_block_col_num < NUM_COL) return emergency_block_col_num;
	emergency_block_col_num = block_diagonal_three();
	if (emergency_block_col_num >= 0 && emergency_block_col_num < NUM_COL) return emergency_block_col_num;
	emergency_block_col_num = block_horizontal_three();
	if (emergency_block_col_num >= 0 && emergency_block_col_num < NUM_COL) return emergency_block_col_num;
	return 1;
}

void refresh(){
	for (int i = 0; i < NUM_COL; i++){
		for (int j = 0; j < NUM_ROW; j++){
			Board[i][j].Sensitivity = NONE;
			Board[i][j].Forbidden = NONE;
		}
	}
}

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
	    if (Board[i][row_num].Player == Lala) return ;
	    else if (Board[i][row_num].Player == Tabuki) Tabuki_spot_num++;
	}
	if (Tabuki_spot_num >=2 ){
		for(int i=col_num; i<col_num + 4; i++){
			if (Board[i][row_num].Player == NONE && Board[i][row_num - 1].Player == NONE) Board[i][row_num - 1].Forbidden = FS;
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
	    if (Board[col_num + i][row_num + i].Player == Lala) return ;
	    else if (Board[col_num + i][row_num + i].Player == Tabuki) Tabuki_spot_num++;
	}
	if (Tabuki_spot_num >=2){
		for(int i=0; i< 4; i++){
			if (Board[col_num + i][row_num + i].Player == NONE && (row_num + i - 1) >= 0 && Board[col_num+ i][row_num + i - 1].Player == NONE) Board[col_num + i][row_num + i - 1].Forbidden = FS;
		}
	}	
}

void markforbidden_dia2(int col_num, int row_num){
	char Tabuki_spot_num = 0;
	for(int i=0; i< 4; i++){
	    if (Board[col_num - i][row_num + i].Player == Lala) return ;
	    else if (Board[col_num - i][row_num + i].Player == Tabuki) Tabuki_spot_num++;
	}
	if (Tabuki_spot_num >=2){
		for(int i=0; i< 4; i++){
			if (Board[col_num - i][row_num + i].Player == NONE && (row_num + i - 1) >= 0 && Board[col_num - i][row_num + i - 1].Player == NONE) Board[col_num - i][row_num + i - 1].Forbidden = FS;
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
			if (Board[i][j].Player == Tabuki && Board[i][j+1].Player == Tabuki && Board[i][j+2].Player == Tabuki && Board[i][j+3].Player == NONE) {num_block++; blockv3_col_num = i;}
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
		if (Board[col_num + i][row_num + i].Player == Lala) return 4;//Connecting four not detected for the specific sequence
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
		if (Board[col_num - i][row_num + i].Player == Lala) return 4;//Connecting four not detected for the specific sequence
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
				if (Board[i+k][j].Player == Tabuki) Tabuki_spot_num++;
			}
			if (Tabuki_spot_num == 3){
				for(int k = 0; k < 4; k ++){
					if (Board[i+k][j].Player == NONE && (j == 0 || Board[i+k][j - 1].Player != NONE )) {num_block ++; blockh3_col_num = i+k;}
				}
			}
		}
	}
	if (num_block > 1) return blockh3_col_num;
	else if (num_block == 1) return blockh3_col_num;
	return 8;
}