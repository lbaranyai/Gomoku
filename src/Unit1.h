//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Graphics.hpp>
//---------------------------------------------------------------------------
// Board size
const int FIELDX = 30;
const int FIELDY = 25;
// Steps
const int MAXSTEP = FIELDX*FIELDY/2;
// Step coordinates
typedef struct {
 int UX,UY; // user
 int CX,CY; // computer
} Coordinates;

class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TImage *Image1;
	TButton *Button1;
	TListBox *ListBox1;
	TLabel *Label1;
	TPanel *Panel1;
	TLabel *Label2;
	TLabel *Label3;
	TRadioGroup *RadioGroup1;
	TImage *Empty;
	TImage *TileUser;
	TImage *TileComputer;
	TButton *Button2;
	TTimer *Timer1;
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall Restart(void);
	int  __fastcall GetCell(int x,int y);
	int  __fastcall CountSameDir(int x,int y,int dx,int dy);
	int  __fastcall CountScoreDir(int x,int y,int dx,int dy,int radius,int fv);
	int  __fastcall CountScore(int x,int y,int fv);
	void __fastcall Evaluate(void);
    int  __fastcall TryWinner(int x,int y,int fv);
	void __fastcall TestForWin(int x,int y);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall RadioGroup1Click(TObject *Sender);
	void __fastcall Image1MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
		  int X, int Y);
	void __fastcall ShowCell(int x,int y);
	void __fastcall MarkCell(int x,int y,int fv);
	void __fastcall AddMoveList(void);
	void __fastcall ResetMoveList(void);
	void __fastcall HighlightMove(int idx);
	void __fastcall ListBox1Click(TObject *Sender);
private:	// User declarations
	TImage **Tiles;
	TCanvas *drw;
	int **Board;
	Coordinates *GameSteps;
	unsigned int TimerTick;
	int StepCount,HighlightIdx;
	bool Running;
	int ComputerX,ComputerY;
    UnicodeString msgPlayerWin,msgComputerWin;
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
