/* 
�HGTK+��@��²����a�p�����C��
�w�]�j�p��10X20(WIDTH X HEIGHT)�A�w�]���u�Ƭ�30��(COUNT_OF_BOMB)�C 
�C���覡�򥻤W�PWindows���ت���a�p�ۦP�A���P�Ʀr�|�����P�C��C 
������}�ҡA �k��]�X�СA����h�i��ܬO�_���s�G���C 
�D�������ثe�������u�ƩM�C���ɶ����T���A�i�����H�U�Ԧ����վ㬵�u�ƥءC 
�C��������A�i��ܬO�_���s�C���A�ε����C���C 

update:2010.5.13
*/
 
#include <gtk/gtk.h>
#include <glib.h> 
#include <stdlib.h>
#include <stdio.h>
#include <time.h> 

#define WIDTH 20  // �e 
#define HEIGHT 10 // �� 
#define COUNT_OF_BOMB 30 // ���u�`�� 

enum buttonState { // ���s�O��ܩ����� 
    show ,
    hide ,
    lastHide , // �̪�~���ê�
    lastShow // �̪�~��ܪ� 
} ;

struct position { // (x,y)
    int x ;
    int y ;    
} ;
typedef struct position pos ;



// ========================= declaration =============================

// global variables

GtkWidget *gButton[WIDTH][HEIGHT] ;  // ���������s 
GtkWidget *gLabel[WIDTH][HEIGHT] ;  // table��������label  
GtkWidget *gLabelOfBomb ; // ��ܪ����u��(���u�`��-�X���`��) 
GtkWidget *gLabelOfTime ; // ��ܪ��C���ɶ� 
enum buttonState gButtonState[WIDTH][HEIGHT] ; // �����������s��show/hide���A 
gboolean gHaveBomb[WIDTH][HEIGHT] ; // �C�Ӧ�m���ФW���L���u���O�� 
gboolean gGameOver = FALSE ; // �O�_��쬵�u 
int gBombNumber[WIDTH][HEIGHT] ; // �C�Ӧ�m�����u�Ʀr 
int gCountOfBomb = COUNT_OF_BOMB ; // ���u�`�� 

  
// functions

char *utf8( char *str ) ; // �ѨM�L�k��ܤ��媺�~��
GString *numTostr( int num ) ; // �Ʀr��r��

void showQuitQuestion( GtkWidget *button, gpointer window, const gchar *message, 
                       const gchar *fileName ) ; // ���X�O�_���� or ���������D���� 
void showAgainQuestion( GtkWidget *button, gpointer window ) ; // ���X�O�_���Ӫ����D���� 
void button_clicked_callback( GtkWidget *button, GdkEventButton *event, 
                              gpointer window ) ; // �B�z����������D�禡 
void timeout_callback( GtkWidget *label ) ; // �C�@�q�ɶ������label 
gboolean combo_changed(GtkComboBox *comboBox, gpointer window ) ; // �]����������u�ư���� 

void landExpansion( int x, int y ) ; //  �H���j�覡�}���A�X�i�ثe�g�a 
void landExpansionStepTwo() ; // �}æ�X�g�ĤG�B 
void hideAroundButton( int x, int y ) ; // �N(x,y)�|�P�����s�]����hide
void lastHideToHide() ; // �N�������s��lastHide���A�ରhide���A
void showAllBomb() ; // ��쬵�u���z���� 
void buttonClicked( int x, int y ) ; // �I�U���s�᪺���� 
int countOfSurroundingsBomb( int x, int y ) ; // �p��P�򪺬��u�ƨæ^�� 
void reset( GtkWidget *window, int countOfBomb, int height, int width ) ; // ���s�]�w�C���A�����٭�

void setGameOver( gboolean now ) ; // �]�m�C�����A�]true:����, FALSE:�������^ 
void setBomb( int x, int y, int set ) ; // �]�m�U�O���u
void setAllBomb( int countOfBomb ) ; // �]�m���u 
void setButtonState( int x, int y , enum buttonState state ) ; // �]�m����(x,y)�����A
void setBombNumber( int x, int y, int num ) ; // �]�m���u�Ʀr
void setLabel( int x, int y, GtkWidget *label ) ; // �]�m(x,y)��m��label(�D�n�Oreset��)
void setDefineCountOfBomb( int count ) ; // �]�m�ثe���u���˸m�`�� 
void setLabelOfTime( GtkWidget *label ) ; // �]�m�ثe���u���Ʀr( ���u�� - �X�м� ) 
void setLabelOfTimeStr( const gchar *str ) ; // �]�m�ثe���u���Ʀr( ���u�� - �X�м� ) 
void setLabelOfBomb( GtkWidget *label ) ; // �]�m�ثe���u���Ʀr( ���u�� - �X�м� ) 
GtkWidget *setComboBox() ; // �]�mcomboBox�A�i�����קוּ�u�ƥ� 

int getRandNumber() ; // �����H���� 
int getCountOfBomb() ; // ���o�ثe���u�`�� 
pos *getRandPosition() ; // ���o�H�����y�Ц�m(x,y)  
enum buttonState getButtonState( int x, int y ) ; // ���o����(x,y)���ثe���A 
int getBombNumber( int x, int y ) ; // ���o(x,y)�����u�Ʀr
GtkWidget *getLabel( int x, int y ) ; // ���o(x,y)��m��label(�D�n�Oreset��) 
int getDefineCountOfBomb() ; // ���o�ثe���u���˸m�`�� 
int getLabelOfTimeNum() ; // �^�ǥثe���u���Ʀr( ���u�� - �X�м� ) 
const gchar *getLabelOfTimeStr() ; // ���o�ثe���u���Ʀrstr( ���u�� - �X�м� ) 
GtkWidget *getLabelOfBomb() ; // �^�ǥثe���u���Ʀr( ���u�� - �X�м� ) 

gboolean isGameOver() ; // �O�_�w�g��쬵�u�@�ϹC�������H 
gboolean isAllBombOK() ; // �O�_���p���\�H 
gboolean isBomb( int x, int y ) ; // (x,y)�o�Ӧ�m�O�_�����u 







// ========================= definition =============================


char *utf8( char *str )
// �ѨM�L�k��ܤ��媺�~��
{
    return g_locale_to_utf8( str, -1, NULL, NULL, NULL);
} 

void setDefineCountOfBomb( int count )
// �]�m�ثe���u���˸m�`�� 
{
    gCountOfBomb = count ;  
}

int getDefineCountOfBomb()
// ���o�ثe���u���˸m�`�� 
{
    return gCountOfBomb ; 
}

void setLabelOfTime( GtkWidget *label )
// �]�m�ثe���u���Ʀr( ���u�� - �X�м� ) 
{
    gLabelOfTime = label ;  
}

void setLabelOfTimeStr( const gchar *str )
// �]�m�ثe���u���Ʀr( ���u�� - �X�м� ) 
{
    gtk_label_set_text( GTK_LABEL(gLabelOfTime), str ) ;  
}

const gchar *getLabelOfTimeStr()
// �]�m�ثe���u���Ʀr( ���u�� - �X�м� ) 
{
    return gtk_label_get_text( GTK_LABEL(gLabelOfTime) ) ;  
}

int getLabelOfTimeNum()
// �^�ǥثe���u���Ʀr( ���u�� - �X�м� )  
{
    return atoi( gtk_label_get_text( GTK_LABEL(gLabelOfTime) ) ) ;   
}

void setLabelOfBomb( GtkWidget *label )
// �]�m�ثe���u���Ʀr( ���u�� - �X�м� ) 
{
    gLabelOfBomb = label ;  
}

void setLabelOfBombStr( const gchar *str )
// �]�m�ثe���u���Ʀr( ���u�� - �X�м� ) 
{
    gtk_label_set_text( GTK_LABEL(gLabelOfBomb), str ) ;  
}

int getLabelOfBombNum()
// �^�ǥثe���u���Ʀr( ���u�� - �X�м� )  
{
    return atoi( gtk_label_get_text( GTK_LABEL(gLabelOfBomb) ) ) ;   
}

GtkWidget *getLabelOfBomb()
// �^�ǥثe���u���Ʀr( ���u�� - �X�м� )  
{
    return gLabelOfBomb;   
}

gboolean isGameOver()
// �O�_�w�g��쬵�u�@�ϹC�������H 
{
    return gGameOver ? TRUE : FALSE ;
}

gboolean isAllBombOK()
// �O�_���p���\�H 
{
    gboolean ok = TRUE ;
    
    int i, j ;
    for ( i = 0 ; i < HEIGHT ; i ++ )
        for ( j = 0 ; j < WIDTH ; j ++ ) 
            if ( !isBomb( j, i ) && getButtonState( j, i ) == show )
                ok = FALSE ; // �p�G���D�p�ϫo�����}�A�h���p�|�����\ 
    
    return ok ;
}

void setGameOver( gboolean now )
// �]�m�C�����A�]true:����, FALSE:�������^ 
{
    gGameOver = now ;
}


GString *numTostr( int num )
// �Ʀr��r��
{ 
    GString *numOfStr = g_string_new( "" )  ;
    g_string_printf( numOfStr, "%d", num); // �N�Ʀr�ର�r��s�JnumOfStr 
    return numOfStr ;
}


int getRandNumber()
// �����H���� 
{
    int num = 0 ; 
    return (rand()%(WIDTH*HEIGHT)) ;    
}

pos *getRandPosition()
// ���o�H�����y�Ц�m(x,y)  
{
    pos *p = malloc( sizeof( pos ) ); 
    int num = getRandNumber() ;
    
    p->x = num / HEIGHT ; 
    p->y = num % HEIGHT ; //- ( WIDTH * p->x ) ;  
    
    return p ; 
}

gboolean isBomb( int x, int y )
// (x,y)�o�Ӧ�m�O�_�����u 
{
    return gHaveBomb[x][y] ? TRUE : FALSE ;
} 

void setBomb( int x, int y, int set )
// �]�m�U�O���u
{
    gHaveBomb[x][y] = set ;
} 

int getCountOfBomb()
// ���o�ثe���u�`�� 
{
    int i, j, count = 0 ;
    for ( i = 0 ; i < HEIGHT ; i ++ )
        for ( j = 0 ; j < WIDTH ; j ++ )    
            if ( isBomb( j, i ) )
                count ++ ;
                
    return count ; 
}

void setAllBomb( int countOfBomb ) 
// �]�m���u 
{
    int i, j ;
    pos *p = NULL ;
    for ( i = 0 ; i < HEIGHT ; i ++ )
        for ( j = 0 ; j < WIDTH ; j ++ ) 
            setBomb( j, i, FALSE ) ;
   
    for ( i = 0 ; i < countOfBomb  ; i ++ ) {
        p = getRandPosition() ;
        setBomb( p->x, p->y, TRUE ) ;
        free( p ) ; 
    }
     
    for ( i = getCountOfBomb() ; i < countOfBomb ; i = getCountOfBomb() ) {
        // �]�������H���ƭ��ƨ�A�ҥH�W�[�o�Ӿ��� 
        p = getRandPosition() ;
        setBomb( p->x, p->y, TRUE ) ;
        free( p ) ;        
    } 
} 

void setLabel( int x, int y, GtkWidget *label )
// �]�m�ثetable(�D�n�Oreset��) 
{
    gLabel[x][y] = label ;    
}

GtkWidget *getLabel( int x, int y )
// ���o�]�m�ثetable(�D�n�Oreset��) 
{
    return gLabel[x][y] ;   
}

void setButtonState( int x, int y , enum buttonState state )
// �]�m����(x,y)�����A
{
    gButtonState[x][y] = state ;   
} 

enum buttonState getButtonState( int x, int y )
// ���o����(x,y)���ثe���A 
{
    return gButtonState[x][y] ;
}

void setBombNumber( int x, int y, int num )
// �]�m���u�Ʀr
{
    gBombNumber[x][y] = num ;
} 

int getBombNumber( int x, int y )
// ���o(x,y)�����u�Ʀr
{
    return gBombNumber[x][y] ;
} 

void landExpansion( int x, int y )
//  �H���j�覡�}���A�X�i�ثe�g�a 
{
    
    if ( getButtonState( x, y ) != hide && 
         getButtonState( x, y ) != lastHide && 
         !isBomb( x, y ) && 
         getBombNumber( x,y ) == 0 ) {
        setButtonState( x, y, lastHide ) ; // �]��lastHide�H��KlandExpansionStepTwo()
        
        if ( x+1 < WIDTH && y+1 < HEIGHT )
            landExpansion( x+1, y+1 ) ;
        if ( x+1 < WIDTH && y-1 >= 0 )
            landExpansion( x+1, y-1 ) ;
        if ( x+1 < WIDTH )
            landExpansion( x+1, y ) ;
        if ( x-1 >= 0 && y+1 < HEIGHT )
            landExpansion( x-1, y+1 ) ;
        if ( x-1 >= 0 && y-1 >= 0 )
            landExpansion( x-1, y-1 ) ;
        if ( x-1 >= 0 )
            landExpansion( x-1, y ) ;
        if ( y+1 < HEIGHT )
            landExpansion( x, y+1 ) ;
        if ( y-1 >= 0 )
            landExpansion( x, y-1 ) ;
    }
}

void hideAroundButton( int x, int y ) 
// �N(x,y)�|�P�����s�]����hide
{
    if ( x+1 < WIDTH && y+1 < HEIGHT && getButtonState( x+1, y+1 ) != lastHide )
        setButtonState( x+1, y+1, hide ) ;
    if ( x+1 < WIDTH && y-1 >= 0 && getButtonState( x+1, y-1 ) != lastHide )
        setButtonState( x+1, y-1, hide ) ;
    if ( x+1 < WIDTH && getButtonState( x+1, y ) != lastHide )
        setButtonState( x+1, y, hide ) ;
    if ( x-1 >= 0 && y+1 < HEIGHT && getButtonState( x-1, y+1 ) != lastHide )
        setButtonState( x-1, y+1, hide ) ;
    if ( x-1 >= 0 && y-1 >= 0 && getButtonState( x-1, y-1 ) != lastHide )
        setButtonState( x-1, y-1, hide ) ;
    if ( x-1 >= 0 && getButtonState( x-1, y ) != lastHide )
        setButtonState( x-1, y, hide ) ;
    if ( y+1 < HEIGHT && getButtonState( x, y+1 ) != lastHide )
        setButtonState( x, y+1, hide ) ;
    if ( y-1 >= 0 && getButtonState( x, y-1 ) != lastHide )
        setButtonState( x, y-1, hide ) ;
    
} 

void lastHideToHide()
// �N�������s��lastHide���A�ରhide���A
{
    int i = 0 , j = 0 ;
    for ( i = 0 ; i < HEIGHT ; i ++ )
        for ( j = 0 ; j < WIDTH ; j ++ )
            if ( getButtonState( j, i ) == lastHide )
                setButtonState( j, i, hide ) ; 
} 

void landExpansionStepTwo()
// �}æ�X�g�ĤG�B 
{
    int i = 0 , j = 0 ;
    for ( i = 0 ; i < HEIGHT ; i ++ ) 
        for ( j = 0 ; j < WIDTH ; j ++ ) 
            if ( getButtonState( j, i ) == lastHide ) 
                hideAroundButton( j, i ) ;        
} 

void showAllBomb()
// ��쬵�u���z���� 
{
    int i = 0 , j = 0 ;
    for ( i = 0 ; i < HEIGHT ; i ++ ) 
        for ( j = 0 ; j < WIDTH ; j ++ ) 
            if ( isBomb( j, i ) ) 
                setButtonState( j, i, hide ) ;
}

void buttonClicked( int x, int y )
// �I�U���s�᪺���� 
{
    if ( isBomb( x, y ) ) {
        showAllBomb() ;
        setGameOver( TRUE ) ;
    }
    else if ( getBombNumber( x, y ) > 0 ) 
        setButtonState( x, y , hide ) ;
    else {
        landExpansion( x, y ) ; 
        landExpansionStepTwo() ; // ���}�P��
        lastHideToHide() ; // �̫�n�NlastHide -> hide 
    }    
}

int countOfSurroundingsBomb( int x, int y )
// �p��P�򪺬��u�ƨæ^�� 
{
    int countOfBomb = 0 ;
    
    if ( isBomb( x, y ) )
        countOfBomb ++ ;
    
    if ( x+1 < WIDTH && y+1 < HEIGHT )
        if ( isBomb( x+1, y+1 ) )
            countOfBomb ++ ; 
    if ( x+1 < WIDTH )
        if ( isBomb( x+1, y ) )
            countOfBomb ++ ; 
    if ( x+1 < WIDTH && y-1 >= 0 )
        if ( isBomb( x+1, y-1 ) )
            countOfBomb ++ ; 
    if ( y+1 < HEIGHT )
        if ( isBomb( x, y+1 ) )
            countOfBomb ++ ; 
    if ( y-1 >= 0 )
        if ( isBomb( x, y-1 ) )
            countOfBomb ++ ; 
    if ( x-1 >= 0 && y+1 < HEIGHT )
        if ( isBomb( x-1, y+1 ) )
            countOfBomb ++ ; 
    if ( x-1 >= 0 )
        if ( isBomb( x-1, y ) )
            countOfBomb ++ ; 
    if ( x-1 >= 0 && y-1 >= 0 )
        if ( isBomb( x-1, y-1 ) )
            countOfBomb ++ ; 
        
    return countOfBomb ;
}


void showQuitQuestion( GtkWidget *button, gpointer window, 
                       const gchar *message, const gchar *fileName  )
// ���X�O�_���� or ���������D���� 
{
    GtkWidget *dialog;
    int result = 0 ;
    GtkWidget *image = gtk_image_new_from_file( fileName ) ;
    
    
    dialog = gtk_message_dialog_new( GTK_WINDOW(window),
                GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_QUESTION,
                GTK_BUTTONS_YES_NO, 
                message );
                
    gtk_message_dialog_set_image( GTK_MESSAGE_DIALOG( dialog ), image ) ;            
    
    gtk_message_dialog_format_secondary_text(
         GTK_MESSAGE_DIALOG(dialog), utf8("�O�_���s�i��C���H�O�G���ӡ@�U�@�_�G����"));
    gtk_window_set_title(GTK_WINDOW(dialog), utf8("�C������"));
 
    result = gtk_dialog_run(GTK_DIALOG(dialog)) ; 
  
    if ( result == GTK_RESPONSE_YES ) 
        reset( window, getDefineCountOfBomb(), HEIGHT, WIDTH ) ; // ��YES�A�C�����s�}�l 
    else if ( result == GTK_RESPONSE_NO )
        gtk_widget_destroy( window ) ; // ��NO�A�����C������  
  
    gtk_widget_destroy(dialog);
}

void showAgainQuestion(GtkWidget *button, gpointer window )
// ���X�O�_���Ӫ����D���� 
{
    GtkWidget *dialog;
    int result = 0 ;
    
    dialog = gtk_message_dialog_new( GTK_WINDOW(window),
                GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_QUESTION,
                GTK_BUTTONS_YES_NO, 
                utf8("�O�_���s�i��C���H") );
    
    gtk_message_dialog_format_secondary_text(
         GTK_MESSAGE_DIALOG(dialog), utf8("�O�G���ӡ@�U�@�_�G��^") );
    gtk_window_set_title(GTK_WINDOW(dialog), utf8("�C���Ȱ�"));
 
    result = gtk_dialog_run(GTK_DIALOG(dialog)) ; 
  
    if ( result == GTK_RESPONSE_YES ) 
        reset( window, getDefineCountOfBomb(), HEIGHT, WIDTH ) ; // ��YES�A�C�����s�}�l 
    else if ( result == GTK_RESPONSE_NO ) ; // ��No�A�S�����A��^�D�����C 
  
    gtk_widget_destroy(dialog);
}

void button_clicked_callback(GtkWidget *button, GdkEventButton *event, gpointer window )
// �B�z����������D�禡 
{
    int x = 0, y = 0 ;
    int i = 0, j = 0 ;
    int bombNum = 0 ;
    gboolean over = FALSE ;
    GString *str = g_string_new( "" ) ;
    GtkWidget *imageFlag, *imageNormal ;      
    
    imageFlag = gtk_image_new_from_file("48px-Flag-es.svg.png") ; 
    imageNormal = gtk_image_new_from_file("48px-Edit-copy_green.svg.png") ; 
    
    for ( i = 0 ; i < HEIGHT && !over ; i ++ )
        for ( j = 0 ; j < WIDTH && !over ; j ++ ) 
            if ( gButton[j][i] == button ) // ���o�P���쪺button����m 
                over = TRUE ; 
    
    x = j-1 ; // (x,y)
    y = i-1 ; 

    if(event->type == GDK_BUTTON_PRESS && event->button == 3) { // ���U�k��
        
        if ( !strcmp( gtk_button_get_label( GTK_BUTTON( gButton[x][y] ) ), "@" ) ) {
            gtk_button_set_label( GTK_BUTTON( gButton[x][y] ), "?" ) ;
            bombNum = getLabelOfBombNum() + 1 ;
            g_string_printf( str, "%d", bombNum);
            setLabelOfBombStr( str->str ) ;
            
        }
        else if ( !strcmp( gtk_button_get_label( GTK_BUTTON( gButton[x][y] ) ), "?" ) )
            gtk_button_set_label( GTK_BUTTON( gButton[x][y] ), "" ) ;
        else {
            gtk_button_set_label( GTK_BUTTON( gButton[x][y] ), "@" ) ;
            bombNum = getLabelOfBombNum() - 1 ;
            g_string_printf( str, "%d", bombNum);
            setLabelOfBombStr( str->str ) ; 
        }
    }
    else if(event->type == GDK_BUTTON_PRESS && event->button == 2) // ���U����
        showAgainQuestion( button, window ) ;   
    else if(event->type == GDK_BUTTON_PRESS && event->button == 1) { // ���U���� 
        buttonClicked( x, y ) ;
    
        for ( i = 0 ; i < HEIGHT ; i ++ )
            for ( j = 0 ; j < WIDTH ; j ++ )
                if ( getButtonState( j, i ) == hide )
                    gtk_widget_hide( gButton[j][i] ) ;
                    
        if ( isGameOver() ) { // �p�G���쬵�u�N�|���X���D���� 
            g_string_append( str, utf8("���p���ѡA���a�����I") );
            g_string_append( str, utf8(" ( �C���ɶ�: ") );
            g_string_append( str, getLabelOfTimeStr() );
            g_string_append( str, utf8(" �� ) ") );
            showQuitQuestion( button, window, str->str, "flag.png" ) ; 
        }
        else if ( isAllBombOK() ) { // �O�_���p����  
            g_string_append( str, utf8("���ߧA���p�����I�I�I") );
            g_string_append( str, utf8(" ( �C���ɶ�: ") ) ;
            g_string_append( str, getLabelOfTimeStr() );
            g_string_append( str, utf8(" �� ) ") );
            showQuitQuestion( button, window, str->str, "leaf.gif" ) ;
        }
    }    
}

const gchar *getColor( int num )
// �^�����ӨϥΪ��C�� 
{
    if ( num == 0 )
        return "<span foreground=\"#333333\">" ; // �¦� 
    else if ( num == 1 )
        return "<span foreground=\"#3333FF\">" ; // �Ŧ�
    else if ( num == 2 )
        return "<span foreground=\"#009900\">" ; // ��� 
    else if ( num == 3 )
        return "<span foreground=\"#FF0000\">" ; // ���� 
    else if ( num == 4 )
        return "<span foreground=\"#6600FF\">" ; // ���� 
    else if ( num == 5 )
        return "<span foreground=\"#996600\">" ; // ��� 
    else if ( num == 6 )
        return "<span foreground=\"#999900\">" ; // ���� 
    else if ( num == 7 )
        return "<span foreground=\"#00FFCC\">" ; // �C�� 
    else
        return "<span foreground=\"#333333\">" ; // �¦� 
}

void reset( GtkWidget *window, int countOfBomb, int height, int width )
// ���s�]�w�C���A�����٭�
{
    GtkWidget *table;
    GtkWidget *label;
    int i = 0, j = 0, count = 0 ;
    GString *numOfStr = g_string_new("");
    
    srand(time(NULL)) ; // ���]�ü� 
    
    setGameOver( FALSE ) ; // ���s�}�l�C�� 
    setLabelOfTimeStr( "0" ) ; // �C���ɶ��k�s 
    setAllBomb( countOfBomb ) ; // �]�m���u 
    
    for ( i = 0 ; i < height ; i ++ ) {
        for ( j = 0 ; j < width ; j ++ ) { 
            gtk_button_set_label( GTK_BUTTON( gButton[j][i] ), "" ) ; // ��W����*������ 
            
            setButtonState( j, i, show ) ; // �������s����ܥX�� 
            count = countOfSurroundingsBomb( j, i ) ; // �P�򬵼u�ƥ[�` 
            setBombNumber( j, i, count ) ; // �g�J�����ܼ� 
            g_string_printf( numOfStr, "%d", count); // �N�Ʀr�ର�r��s�JnumOfStr 
            
            if ( isBomb(j , i ) )
                 // �����u�����ФW�O��(�W���\�W���s�A�ҥH�ݤ���) 
                g_string_append(numOfStr, "(@)");  
            
            
            g_string_prepend( numOfStr, getColor( count ) ) ; // ���o�Ӧ����C�� 
            g_string_append( numOfStr, "</span>" ) ;
            gtk_label_set_markup(GTK_LABEL(getLabel( j, i )), numOfStr->str ); // ��W�C�� 

        }
    }   
    g_string_printf( numOfStr, "%d", countOfBomb );
    setLabelOfBombStr( numOfStr->str ) ; // �٭���ܪ����u�� 
    
    gtk_widget_show_all( window ) ; // ���s�����٭� 
} 

void timeout_callback( GtkWidget *label )
// �C�@�q�ɶ������label 
{
    GString *temp = g_string_new( gtk_label_get_text( GTK_LABEL( label ) ) ) ;
    
    int timeNow = atoi( temp->str ) + 1 ;
    
    g_string_printf( temp, "%d", timeNow );
    gtk_label_set_text( GTK_LABEL( label ), temp->str ) ;  
}

gboolean combo_changed(GtkComboBox *comboBox, gpointer window ) 
// �]����������u�ư���� 
{
    gchar *active = gtk_combo_box_get_active_text(comboBox) ;
    int count = 0 ;
    
    count =  atoi( active ) ; // �N�ҿ諸���u�Ƭ����_�� 
    setDefineCountOfBomb( count ) ;
    
    reset( window, count, HEIGHT, WIDTH ) ; // ���s�]�w�C�� 

    // gtk_combo_box_set_active( comboBox, 0 ); // �̵M��ܲĤ@�� 
}


GtkWidget *setComboBox()
// �]�mcomboBox�A�i�����קוּ�u�ƥ�  
{
    GtkWidget *comboBox = gtk_combo_box_new_text();
    gtk_combo_box_append_text(GTK_COMBO_BOX(comboBox), utf8( "���m�a�p��" ));
    gtk_combo_box_append_text(GTK_COMBO_BOX(comboBox), "10");
    gtk_combo_box_append_text(GTK_COMBO_BOX(comboBox), "20");
    gtk_combo_box_append_text(GTK_COMBO_BOX(comboBox), "30");
    gtk_combo_box_append_text(GTK_COMBO_BOX(comboBox), "40");
    gtk_combo_box_append_text(GTK_COMBO_BOX(comboBox), "50");
    gtk_combo_box_append_text(GTK_COMBO_BOX(comboBox), "60");
    gtk_combo_box_append_text(GTK_COMBO_BOX(comboBox), "70");
    gtk_combo_box_append_text(GTK_COMBO_BOX(comboBox), "80");
    gtk_combo_box_append_text(GTK_COMBO_BOX(comboBox), "90");
    gtk_combo_box_set_active(GTK_COMBO_BOX(comboBox), 0);
    
    return comboBox ;
}

int main(int argc, char *argv[]) {
    GtkWidget *window ;
    GtkWidget *table ;
    GtkWidget *label ;
    GtkWidget *comboBox;
    GtkWidget *vbox, *hbox ;
    GtkWidget *labelOfTemp ;
    GtkWidget *labelOfBomb, *labelOfBombStr ;
    GtkWidget *labelOfTime, *labelOfTimeStr, *labelOfTimeStr2 ;
    GtkWidget *image ;
    
    int i, j, k;
    int countOfBomb = 0 ; // �P�򬵼u�[�` 

    GString *numOfStr = g_string_new("");
    GString *str = g_string_new("");
    
    srand(time(NULL)) ; // ���]�ü� 

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), utf8("��a�p"));
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 300);
    table = gtk_table_new(HEIGHT, WIDTH, TRUE);
    
    setAllBomb( getDefineCountOfBomb() ) ; // �]�m���u 
    
    for ( i = 0 ; i < HEIGHT ; i ++ ) {
        for ( j = 0 ; j < WIDTH ; j ++ ) { 
            setButtonState( j, i, show ) ; // �������s����ܥX�� 
            countOfBomb = countOfSurroundingsBomb( j, i ) ; // �P�򬵼u�ƥ[�` 
            setBombNumber( j, i, countOfBomb ) ; // �g�J�����ܼ� 
            g_string_printf( numOfStr, "%d", countOfBomb); // �N�Ʀr�ର�r��s�JnumOfStr 
            
            if ( isBomb(j , i ) )
                 // �����u�����ФW�O��(�W���\�W���s�A�ҥH�ݤ���) 
                g_string_append(numOfStr, "(@)");  
            
            label = gtk_label_new("") ;  
            
            g_string_prepend( numOfStr, getColor( countOfBomb ) ) ; // ���o�Ӧ����C�� 
            g_string_append( numOfStr, "</span>" ) ;
            gtk_label_set_markup(GTK_LABEL(label), numOfStr->str ); // ��W�C��
            
            setLabel( j, i, label ) ; // �N�U��label�s�J�����ܼ�gLabel
             
            gButton[j][i] = gtk_button_new_with_label( "" ) ; 
            
            gtk_table_attach_defaults( // �Nbutton��Jtable 
                GTK_TABLE(table), gButton[j][i], j, j + 1, i, i + 1);
            gtk_table_attach_defaults( // �Nlabel��Jtable 
                GTK_TABLE(table), label, j, j + 1, i, i + 1);  
        }
    } 
    
    comboBox = setComboBox() ; // �]�mcomboBox�A�i�����קוּ�u�ƥ� 
    
    labelOfBombStr = gtk_label_new(NULL);
    
    gtk_label_set_markup(GTK_LABEL(labelOfBombStr), \
        utf8( "<span foreground=\"#336699\">�a�p�� :</span>" ) ); // �]�mlabel�r���C�� 
    
    g_string_printf( str, "%d", getDefineCountOfBomb() );
    
    labelOfTimeStr = gtk_label_new( "" ) ;
    gtk_label_set_markup(GTK_LABEL(labelOfTimeStr), \
        utf8( "<span foreground=\"#336699\">     �w�ϥήɶ� :</span>" ) );
    labelOfTimeStr2 = gtk_label_new( utf8( "��   " ) ) ;
    labelOfTemp = gtk_label_new( utf8( "     " ) ) ;
    
    labelOfBomb = gtk_label_new( str->str );
    setLabelOfBomb( labelOfBomb ) ;
    labelOfTime = gtk_label_new("0");
    setLabelOfTime( labelOfTime ) ;
     
    vbox = gtk_vbox_new(FALSE, 5);
    hbox = gtk_hbox_new(FALSE, 5); 
     
    gtk_box_pack_start(GTK_BOX(hbox), labelOfBombStr, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox), labelOfBomb, FALSE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hbox), labelOfTimeStr, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox), labelOfTime, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(hbox), labelOfTimeStr2, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox), labelOfTemp, FALSE, FALSE, 75);
    gtk_box_pack_start(GTK_BOX(hbox), comboBox, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), table, TRUE, TRUE, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox); 
    
    for ( i = 0 ; i < HEIGHT ; i ++ )
        for ( j = 0 ; j < WIDTH ; j ++ ) 
            g_signal_connect(GTK_OBJECT(gButton[j][i]), "button-press-event",
                                GTK_SIGNAL_FUNC(button_clicked_callback), window);
                                
    g_signal_connect(GTK_OBJECT(comboBox), "changed", // ����comboBox 
                     G_CALLBACK(combo_changed), window );
                                
    g_signal_connect(GTK_OBJECT(window), "destroy",
                     G_CALLBACK(gtk_main_quit), NULL);
    
    g_timeout_add(1000, (GSourceFunc) timeout_callback, labelOfTime ); // �p��C���ɶ� 

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
  
