//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
// Exit from program
void __fastcall TForm1::Button1Click(TObject *Sender)
{
 Application->Terminate();
}
//---------------------------------------------------------------------------
// Initialize defaults
void __fastcall TForm1::FormCreate(TObject *Sender)
{
 int i,wx,wy;
 UnicodeString AS;

 drw = Image1->Canvas;
 wx = (20 - drw->TextWidth(L"XX"))/2;
 wy = (20 - drw->TextHeight(L"XX"))/2;
 // column numbers
 for (i=0;i<FIELDX;i++) {
  AS.printf(L"%02d",i+1);
  drw->TextOutW(20+wx+i*20,wy,AS);
 }
 // row numbers
 for (i=0;i<FIELDY;i++) {
  AS.printf(L"%02d",i+1);
  drw->TextOutW(wx,20+wy+i*20,AS);
 }
 // prepare tile pictures
 Tiles = new TImage*[3];
 Tiles[0] = Empty;
 Tiles[1] = TileComputer;
 Tiles[2] = TileUser;
 // allocate memory for board
 Board = new int*[FIELDX];
 for (i=0;i<FIELDX;i++) Board[i] = new int[FIELDY];
 // allocate memory for step history
 GameSteps = new Coordinates[MAXSTEP];
 Restart();
 // default message text
 msgPlayerWin = "Gratulálok, Ön nyert!";
 msgComputerWin = "Most én nyertem!";
 // initialize random number generator
 randomize();
}
//---------------------------------------------------------------------------
// Clean up
void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
 int i;
 for (i=0;i<FIELDX;i++) delete[] Board[i];
 delete[] Board;
 delete[] GameSteps;
}
//---------------------------------------------------------------------------
// Show game time
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
 int tmin,tsec;
 UnicodeString AS;

 if (Running==false) return;
 TimerTick++;
 tsec = TimerTick%60;
 tmin = (TimerTick - tsec)/60;
 AS.printf(L"%02d:%02d",tmin,tsec);
 Panel1->Caption = AS;
}
//---------------------------------------------------------------------------
// Reset game, clear table
void __fastcall TForm1::Restart(void)
{
 int x,y;

 Running = false;
 Timer1->Enabled = false;
 TimerTick = 0;
 Panel1->Caption = "00:00";
 for (x=0;x<FIELDX;x++) {
  for (y=0;y<FIELDY;y++) {
   drw->Draw(20+x*20,20+y*20,Empty->Picture->Bitmap);
   Board[x][y] = 0;
  }
 }
 StepCount = 0;
 HighlightIdx = -1;
 ResetMoveList();
}
//---------------------------------------------------------------------------
// Read board value at given position
int  __fastcall TForm1::GetCell(int x,int y)
{
 if (x<0 || x>=FIELDX || y<0 || y>FIELDY) return -1;
 return Board[x][y];
}
//---------------------------------------------------------------------------
// Count the number of same tiles in given direction
int  __fastcall TForm1::CountSameDir(int x,int y,int dx,int dy)
{
 int n,d,t;

 t = GetCell(x,y);
 if (t <= 0) return 0;
 n = 0;
 d = 0;
 do { d++; n++; } while(t==GetCell(x+d*dx,y+d*dy));
 d = 0;
 do { d++; n++; } while(t==GetCell(x-d*dx,y-d*dy));
 return n;
}
//---------------------------------------------------------------------------
// Calculate score for given position and direction
// Weights: +1.0 self, +0.1 empty, -0.5 opposer
int  __fastcall TForm1::CountScoreDir(int x,int y,int dx,int dy,int radius,int fv)
{
 int n,d,t;

 n = 0;
 d = 0;
 do{
  d++;
  t = GetCell(x+d*dx,y+d*dy);
  if (t==fv) { n+=1; }
  else if (t==0) { n+=0.1; }
  else { n-=0.5; }
 }while(d<=radius && (t==0 || t==fv));
 return n;
}
//---------------------------------------------------------------------------
// Calculate the score of the position for next step
// Radius of 5 tile is used
int  __fastcall TForm1::CountScore(int x,int y,int fv)
{
 int i,n,t;
 int dx[] = { 0, 1, 1, 1, 0,-1,-1,-1};
 int dy[] = { 1, 1, 0,-1,-1,-1, 0, 1};

 if (GetCell(x,y)!=0) return -1;
 n = 0;
 for (i=0;i<8;i++) {
  t = CountScoreDir(x,y, dx[i], dy[i], 5, fv);
  n += t*t;
 }
 return n;
}
//---------------------------------------------------------------------------
// Calculate next computer move
void __fastcall TForm1::Evaluate(void)
{
 int x,y,CX,CY,win1,win2,t1,t2,sc;
 bool STOP;

 CX = -1;
 CY = -1;
 sc = -1;
 STOP = false;
 // scanning field
 for (x=0;x<FIELDX && STOP==false;x++) {
  for (y=0;y<FIELDY && STOP==false;y++) {
   if (GetCell(x,y)==0) {
	// try if computer wins
	win1 = TryWinner(x,y,1);
	if (win1>5) { STOP=true; CX=x; CY=y; sc=2e+9; } else {
	 // try if player wins
	 win2 = TryWinner(x,y,2);
	 if (win2>5) { CX=x; CY=y; sc=2e+9; } else {
	  if (win1>3) win1++;
	  if (win2>3) win2++;
	  t1 = CountScore(x,y,1)*win1;
	  t2 = CountScore(x,y,2)*win2;
	  if (t1<t2) { t1 = t2+t1/2; } else { t1 += t2/2; }
	  if (sc<t1) { sc=t1; CX=x; CY=y; } else {
	   if (sc==t1 && random(10)>5) { sc=t1; CX=x; CY=y; }
	  }
	 }
	}
   }
  }
 }
 ComputerX = CX;
 ComputerY = CY;
}
//---------------------------------------------------------------------------
// Test whether given party can win at position
int  __fastcall TForm1::TryWinner(int x,int y,int fv)
{
 int i,n,t;
 int dx[] = { 0, 1, 1, 1};
 int dy[] = { 1, 0, 1,-1};

 if (GetCell(x,y)!=0) return 0;
 Board[x][y] = fv;
 n = 0;
 for (i=0;i<4;i++) {
  t = CountSameDir(x, y, dx[i], dy[i]);
  if (n<t) n=t;
 }
 Board[x][y] = 0;
 return n;
}
//---------------------------------------------------------------------------
// Evaluate position for win
void __fastcall TForm1::TestForWin(int x,int y)
{
 int i,n,t;
 int dx[] = { 0, 1, 1, 1};
 int dy[] = { 1, 0, 1,-1};

 n=0;
 for (i=0;i<4;i++) {
  t = CountSameDir(x, y, dx[i], dy[i]);
  if (n<t) n=t;
 }
 // what if we have winner
 if (n>5) {
  Running = false;
  Timer1->Enabled = false;
  t = GetCell(x,y);
  if (t==2) {
   i = Application->MessageBoxW(msgPlayerWin.c_str(),L"Gomoku",MB_OK);
  }
  if (t==1) {
   i = Application->MessageBoxW(msgComputerWin.c_str(),L"Gomoku",MB_OK);
  }
 }
}
//---------------------------------------------------------------------------
// User input - restart with new board
void __fastcall TForm1::Button2Click(TObject *Sender)
{
 Restart();
}
//---------------------------------------------------------------------------
// User input - change interface language
void __fastcall TForm1::RadioGroup1Click(TObject *Sender)
{
 if (RadioGroup1->ItemIndex == 0) {
  // Lang = Hungarian
  Button1->Caption = "Kilépés";
  Button2->Caption = "Új tábla";
  RadioGroup1->Caption = "Nyelv";
  Label1->Caption = "Lépések";
  Label2->Caption = "Játékos";
  Label3->Caption = "Számítógép";
  msgPlayerWin = "Gratulálok, Ön nyert!";
  msgComputerWin = "Most én nyertem!";
 } else {
  // Lang = English
  Button1->Caption = "Quit";
  Button2->Caption = "New board";
  RadioGroup1->Caption = "Language";
  Label1->Caption = "Moves";
  Label2->Caption = "Player";
  Label3->Caption = "Computer";
  msgPlayerWin = "Congratulations, You won!";
  msgComputerWin = "I won this time!";
 }
}
//---------------------------------------------------------------------------
// User input - mouse click on board
void __fastcall TForm1::Image1MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
		  int X, int Y)
{
 int px,py;

 // test if game has finished - nothing to do
 if (Running==false && TimerTick>0) return;
 // user click starts the game if not running already
 if (Running==false && TimerTick==0) {
  Timer1->Enabled = true;
  Running = true;
 }
 // calculate board coordinates
 px = floor((float)X/20.0)-1;
 py = floor((float)Y/20.0)-1;
 if (px<0 || py<0 || px>=FIELDX || py>=FIELDY) return;
 // test if available
 if (GetCell(px,py)!=0) return;
 // player move
 MarkCell(px,py,2);
 TestForWin(px,py);
 GameSteps[ StepCount ].UX = px;
 GameSteps[ StepCount ].UY = py;
 // if player did not win, computer moves
 if (Running==true) {
  Evaluate();
  MarkCell(ComputerX,ComputerY,1);
  TestForWin(ComputerX,ComputerY);
   GameSteps[ StepCount ].CX = ComputerX;
   GameSteps[ StepCount ].CY = ComputerY;
 }
 AddMoveList();
 HighlightMove(StepCount);
 StepCount++;
}
//---------------------------------------------------------------------------
// Graphics - show cell at given position
void __fastcall TForm1::ShowCell(int x,int y)
{
 if (x<0 || x>=FIELDX || y<0 || y>=FIELDY) return;
 drw->Draw(20+x*20,20+y*20,Tiles[ Board[x][y] ]->Picture->Bitmap);
}
//---------------------------------------------------------------------------
// Mark board cell with given player/computer ID if possible
void __fastcall TForm1::MarkCell(int x,int y,int fv)
{
 if (GetCell(x,y)!=0) return;
 Board[x][y] = fv;
 ShowCell(x,y);
}
//---------------------------------------------------------------------------
// Add coordinates to list of moves
void __fastcall TForm1::AddMoveList(void)
{
 UnicodeString AS;
 if (GameSteps[ StepCount ].UX==-1) return;
 if (GameSteps[ StepCount ].CX==-1) {
  // only user moved - won
  AS.printf(L"X(%02d:%02d)",GameSteps[StepCount].UX+1,GameSteps[StepCount].UY+1);
 } else {
  // user and computer move
  AS.printf(L"X(%02d:%02d) - O(%02d:%02d)",
   GameSteps[StepCount].UX+1,GameSteps[StepCount].UY+1,
   GameSteps[StepCount].CX+1,GameSteps[StepCount].CY+1);
 }
 ListBox1->Items->Add(AS);
 ListBox1->ItemIndex = ListBox1->Items->Count - 1;
}
//---------------------------------------------------------------------------
// Clear list of moves
void __fastcall TForm1::ResetMoveList(void)
{
 int i;

 ListBox1->Clear();
 for (i=0;i<MAXSTEP;i++) {
  GameSteps[i].UX = -1;
  GameSteps[i].CX = -1;
 }
}
//---------------------------------------------------------------------------
// Show move index No. idx
void __fastcall TForm1::HighlightMove(int idx)
{
 int hx,hy;

 if (idx<0 || idx>StepCount) return;
 drw->Pen->Color = clYellow;
 drw->Pen->Mode = pmCopy;
 drw->Pen->Width = 2;
 drw->Brush->Style = bsClear;
 // clear previous highlight
 if (HighlightIdx >= 0) {
  hx = GameSteps[ HighlightIdx ].UX;
  hy = GameSteps[ HighlightIdx ].UY;
  ShowCell(hx,hy);
  hx = GameSteps[ HighlightIdx ].CX;
  hy = GameSteps[ HighlightIdx ].CY;
  if (hx>0) {
   ShowCell(hx,hy);
  }
 }
 // show selected move
 hx = GameSteps[ idx ].UX*20;
 hy = GameSteps[ idx ].UY*20;
 drw->Rectangle(21+hx,21+hy,39+hx,39+hy);
 hx = GameSteps[ idx ].CX*20;
 hy = GameSteps[ idx ].CY*20;
 if (hx>0) {
  drw->Rectangle(21+hx,21+hy,39+hx,39+hy);
 }
 HighlightIdx = idx;
}
//---------------------------------------------------------------------------
// User input - Show selected move
void __fastcall TForm1::ListBox1Click(TObject *Sender)
{
 HighlightMove(ListBox1->ItemIndex);
}
//---------------------------------------------------------------------------

