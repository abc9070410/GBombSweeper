/* 
以GTK+實作的簡易踩地雷視窗遊戲
預設大小為10X20(WIDTH X HEIGHT)，預設炸彈數為30顆(COUNT_OF_BOMB)。 
遊戲方式基本上與Windows內建的踩地雷相同，不同數字會有不同顏色。 
按左鍵開啟， 右鍵設旗標，中鍵則可選擇是否重新佈局。 
主視窗有目前未知炸彈數和遊戲時間的訊息，可直接以下拉式選單調整炸彈數目。 
遊戲結束後，可選擇是否重新遊戲，或結束遊戲。 

update:2010.5.13
*/
 
#include <gtk/gtk.h>
#include <glib.h> 
#include <stdlib.h>
#include <stdio.h>
#include <time.h> 

#define WIDTH 20  // 寬 
#define HEIGHT 10 // 長 
#define COUNT_OF_BOMB 30 // 炸彈總數 

enum buttonState { // 按鈕是顯示或隱藏 
    show ,
    hide ,
    lastHide , // 最近才隱藏的
    lastShow // 最近才顯示的 
} ;

struct position { // (x,y)
    int x ;
    int y ;    
} ;
typedef struct position pos ;



// ========================= declaration =============================

// global variables

GtkWidget *gButton[WIDTH][HEIGHT] ;  // 全部的按鈕 
GtkWidget *gLabel[WIDTH][HEIGHT] ;  // table中全部的label  
GtkWidget *gLabelOfBomb ; // 顯示的炸彈數(炸彈總數-旗標總數) 
GtkWidget *gLabelOfTime ; // 顯示的遊戲時間 
enum buttonState gButtonState[WIDTH][HEIGHT] ; // 紀錄全部按鈕的show/hide狀態 
gboolean gHaveBomb[WIDTH][HEIGHT] ; // 每個位置都標上有無炸彈的記號 
gboolean gGameOver = FALSE ; // 是否踩到炸彈 
int gBombNumber[WIDTH][HEIGHT] ; // 每個位置的炸彈數字 
int gCountOfBomb = COUNT_OF_BOMB ; // 炸彈總數 

  
// functions

char *utf8( char *str ) ; // 解決無法顯示中文的窘境
GString *numTostr( int num ) ; // 數字轉字串

void showQuitQuestion( GtkWidget *button, gpointer window, const gchar *message, 
                       const gchar *fileName ) ; // 跳出是否重來 or 關閉的問題視窗 
void showAgainQuestion( GtkWidget *button, gpointer window ) ; // 跳出是否重來的問題視窗 
void button_clicked_callback( GtkWidget *button, GdkEventButton *event, 
                              gpointer window ) ; // 處理按鍵反應的主函式 
void timeout_callback( GtkWidget *label ) ; // 每一段時間都更改label 
gboolean combo_changed(GtkComboBox *comboBox, gpointer window ) ; // 因應選取的炸彈數做更動 

void landExpansion( int x, int y ) ; //  以遞迴方式開路，擴展目前土地 
void landExpansionStepTwo() ; // 開疆擴土第二步 
void hideAroundButton( int x, int y ) ; // 將(x,y)四周的按鈕也全部hide
void lastHideToHide() ; // 將全部按鈕的lastHide狀態轉為hide狀態
void showAllBomb() ; // 踩到炸彈全爆炸啦 
void buttonClicked( int x, int y ) ; // 點下按鈕後的反應 
int countOfSurroundingsBomb( int x, int y ) ; // 計算周圍的炸彈數並回傳 
void reset( GtkWidget *window, int countOfBomb, int height, int width ) ; // 重新設定遊戲，全部還原

void setGameOver( gboolean now ) ; // 設置遊戲狀態（true:結束, FALSE:不結束） 
void setBomb( int x, int y, int set ) ; // 設置各別炸彈
void setAllBomb( int countOfBomb ) ; // 設置炸彈 
void setButtonState( int x, int y , enum buttonState state ) ; // 設置按鍵(x,y)的狀態
void setBombNumber( int x, int y, int num ) ; // 設置炸彈數字
void setLabel( int x, int y, GtkWidget *label ) ; // 設置(x,y)位置的label(主要是reset用)
void setDefineCountOfBomb( int count ) ; // 設置目前炸彈的裝置總數 
void setLabelOfTime( GtkWidget *label ) ; // 設置目前炸彈的數字( 炸彈數 - 旗標數 ) 
void setLabelOfTimeStr( const gchar *str ) ; // 設置目前炸彈的數字( 炸彈數 - 旗標數 ) 
void setLabelOfBomb( GtkWidget *label ) ; // 設置目前炸彈的數字( 炸彈數 - 旗標數 ) 
GtkWidget *setComboBox() ; // 設置comboBox，可直接修改炸彈數目 

int getRandNumber() ; // 產生隨機數 
int getCountOfBomb() ; // 取得目前炸彈總數 
pos *getRandPosition() ; // 取得隨機的座標位置(x,y)  
enum buttonState getButtonState( int x, int y ) ; // 取得按鍵(x,y)的目前狀態 
int getBombNumber( int x, int y ) ; // 取得(x,y)的炸彈數字
GtkWidget *getLabel( int x, int y ) ; // 取得(x,y)位置的label(主要是reset用) 
int getDefineCountOfBomb() ; // 取得目前炸彈的裝置總數 
int getLabelOfTimeNum() ; // 回傳目前炸彈的數字( 炸彈數 - 旗標數 ) 
const gchar *getLabelOfTimeStr() ; // 取得目前炸彈的數字str( 炸彈數 - 旗標數 ) 
GtkWidget *getLabelOfBomb() ; // 回傳目前炸彈的數字( 炸彈數 - 旗標數 ) 

gboolean isGameOver() ; // 是否已經踩到炸彈　使遊戲結束？ 
gboolean isAllBombOK() ; // 是否掃雷成功？ 
gboolean isBomb( int x, int y ) ; // (x,y)這個位置是否有炸彈 







// ========================= definition =============================


char *utf8( char *str )
// 解決無法顯示中文的窘境
{
    return g_locale_to_utf8( str, -1, NULL, NULL, NULL);
} 

void setDefineCountOfBomb( int count )
// 設置目前炸彈的裝置總數 
{
    gCountOfBomb = count ;  
}

int getDefineCountOfBomb()
// 取得目前炸彈的裝置總數 
{
    return gCountOfBomb ; 
}

void setLabelOfTime( GtkWidget *label )
// 設置目前炸彈的數字( 炸彈數 - 旗標數 ) 
{
    gLabelOfTime = label ;  
}

void setLabelOfTimeStr( const gchar *str )
// 設置目前炸彈的數字( 炸彈數 - 旗標數 ) 
{
    gtk_label_set_text( GTK_LABEL(gLabelOfTime), str ) ;  
}

const gchar *getLabelOfTimeStr()
// 設置目前炸彈的數字( 炸彈數 - 旗標數 ) 
{
    return gtk_label_get_text( GTK_LABEL(gLabelOfTime) ) ;  
}

int getLabelOfTimeNum()
// 回傳目前炸彈的數字( 炸彈數 - 旗標數 )  
{
    return atoi( gtk_label_get_text( GTK_LABEL(gLabelOfTime) ) ) ;   
}

void setLabelOfBomb( GtkWidget *label )
// 設置目前炸彈的數字( 炸彈數 - 旗標數 ) 
{
    gLabelOfBomb = label ;  
}

void setLabelOfBombStr( const gchar *str )
// 設置目前炸彈的數字( 炸彈數 - 旗標數 ) 
{
    gtk_label_set_text( GTK_LABEL(gLabelOfBomb), str ) ;  
}

int getLabelOfBombNum()
// 回傳目前炸彈的數字( 炸彈數 - 旗標數 )  
{
    return atoi( gtk_label_get_text( GTK_LABEL(gLabelOfBomb) ) ) ;   
}

GtkWidget *getLabelOfBomb()
// 回傳目前炸彈的數字( 炸彈數 - 旗標數 )  
{
    return gLabelOfBomb;   
}

gboolean isGameOver()
// 是否已經踩到炸彈　使遊戲結束？ 
{
    return gGameOver ? TRUE : FALSE ;
}

gboolean isAllBombOK()
// 是否掃雷成功？ 
{
    gboolean ok = TRUE ;
    
    int i, j ;
    for ( i = 0 ; i < HEIGHT ; i ++ )
        for ( j = 0 ; j < WIDTH ; j ++ ) 
            if ( !isBomb( j, i ) && getButtonState( j, i ) == show )
                ok = FALSE ; // 如果有非雷區卻未打開，則掃雷尚未成功 
    
    return ok ;
}

void setGameOver( gboolean now )
// 設置遊戲狀態（true:結束, FALSE:不結束） 
{
    gGameOver = now ;
}


GString *numTostr( int num )
// 數字轉字串
{ 
    GString *numOfStr = g_string_new( "" )  ;
    g_string_printf( numOfStr, "%d", num); // 將數字轉為字串存入numOfStr 
    return numOfStr ;
}


int getRandNumber()
// 產生隨機數 
{
    int num = 0 ; 
    return (rand()%(WIDTH*HEIGHT)) ;    
}

pos *getRandPosition()
// 取得隨機的座標位置(x,y)  
{
    pos *p = malloc( sizeof( pos ) ); 
    int num = getRandNumber() ;
    
    p->x = num / HEIGHT ; 
    p->y = num % HEIGHT ; //- ( WIDTH * p->x ) ;  
    
    return p ; 
}

gboolean isBomb( int x, int y )
// (x,y)這個位置是否有炸彈 
{
    return gHaveBomb[x][y] ? TRUE : FALSE ;
} 

void setBomb( int x, int y, int set )
// 設置各別炸彈
{
    gHaveBomb[x][y] = set ;
} 

int getCountOfBomb()
// 取得目前炸彈總數 
{
    int i, j, count = 0 ;
    for ( i = 0 ; i < HEIGHT ; i ++ )
        for ( j = 0 ; j < WIDTH ; j ++ )    
            if ( isBomb( j, i ) )
                count ++ ;
                
    return count ; 
}

void setAllBomb( int countOfBomb ) 
// 設置炸彈 
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
        // 因為有些隨機數重複到，所以增加這個機制 
        p = getRandPosition() ;
        setBomb( p->x, p->y, TRUE ) ;
        free( p ) ;        
    } 
} 

void setLabel( int x, int y, GtkWidget *label )
// 設置目前table(主要是reset用) 
{
    gLabel[x][y] = label ;    
}

GtkWidget *getLabel( int x, int y )
// 取得設置目前table(主要是reset用) 
{
    return gLabel[x][y] ;   
}

void setButtonState( int x, int y , enum buttonState state )
// 設置按鍵(x,y)的狀態
{
    gButtonState[x][y] = state ;   
} 

enum buttonState getButtonState( int x, int y )
// 取得按鍵(x,y)的目前狀態 
{
    return gButtonState[x][y] ;
}

void setBombNumber( int x, int y, int num )
// 設置炸彈數字
{
    gBombNumber[x][y] = num ;
} 

int getBombNumber( int x, int y )
// 取得(x,y)的炸彈數字
{
    return gBombNumber[x][y] ;
} 

void landExpansion( int x, int y )
//  以遞迴方式開路，擴展目前土地 
{
    
    if ( getButtonState( x, y ) != hide && 
         getButtonState( x, y ) != lastHide && 
         !isBomb( x, y ) && 
         getBombNumber( x,y ) == 0 ) {
        setButtonState( x, y, lastHide ) ; // 設為lastHide以方便landExpansionStepTwo()
        
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
// 將(x,y)四周的按鈕也全部hide
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
// 將全部按鈕的lastHide狀態轉為hide狀態
{
    int i = 0 , j = 0 ;
    for ( i = 0 ; i < HEIGHT ; i ++ )
        for ( j = 0 ; j < WIDTH ; j ++ )
            if ( getButtonState( j, i ) == lastHide )
                setButtonState( j, i, hide ) ; 
} 

void landExpansionStepTwo()
// 開疆擴土第二步 
{
    int i = 0 , j = 0 ;
    for ( i = 0 ; i < HEIGHT ; i ++ ) 
        for ( j = 0 ; j < WIDTH ; j ++ ) 
            if ( getButtonState( j, i ) == lastHide ) 
                hideAroundButton( j, i ) ;        
} 

void showAllBomb()
// 踩到炸彈全爆炸啦 
{
    int i = 0 , j = 0 ;
    for ( i = 0 ; i < HEIGHT ; i ++ ) 
        for ( j = 0 ; j < WIDTH ; j ++ ) 
            if ( isBomb( j, i ) ) 
                setButtonState( j, i, hide ) ;
}

void buttonClicked( int x, int y )
// 點下按鈕後的反應 
{
    if ( isBomb( x, y ) ) {
        showAllBomb() ;
        setGameOver( TRUE ) ;
    }
    else if ( getBombNumber( x, y ) > 0 ) 
        setButtonState( x, y , hide ) ;
    else {
        landExpansion( x, y ) ; 
        landExpansionStepTwo() ; // 打開周圍
        lastHideToHide() ; // 最後要將lastHide -> hide 
    }    
}

int countOfSurroundingsBomb( int x, int y )
// 計算周圍的炸彈數並回傳 
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
// 跳出是否重來 or 關閉的問題視窗 
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
         GTK_MESSAGE_DIALOG(dialog), utf8("是否重新進行遊戲？是：重來　｜　否：結束"));
    gtk_window_set_title(GTK_WINDOW(dialog), utf8("遊戲結束"));
 
    result = gtk_dialog_run(GTK_DIALOG(dialog)) ; 
  
    if ( result == GTK_RESPONSE_YES ) 
        reset( window, getDefineCountOfBomb(), HEIGHT, WIDTH ) ; // 按YES，遊戲重新開始 
    else if ( result == GTK_RESPONSE_NO )
        gtk_widget_destroy( window ) ; // 按NO，關閉遊戲視窗  
  
    gtk_widget_destroy(dialog);
}

void showAgainQuestion(GtkWidget *button, gpointer window )
// 跳出是否重來的問題視窗 
{
    GtkWidget *dialog;
    int result = 0 ;
    
    dialog = gtk_message_dialog_new( GTK_WINDOW(window),
                GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_QUESTION,
                GTK_BUTTONS_YES_NO, 
                utf8("是否重新進行遊戲？") );
    
    gtk_message_dialog_format_secondary_text(
         GTK_MESSAGE_DIALOG(dialog), utf8("是：重來　｜　否：返回") );
    gtk_window_set_title(GTK_WINDOW(dialog), utf8("遊戲暫停"));
 
    result = gtk_dialog_run(GTK_DIALOG(dialog)) ; 
  
    if ( result == GTK_RESPONSE_YES ) 
        reset( window, getDefineCountOfBomb(), HEIGHT, WIDTH ) ; // 按YES，遊戲重新開始 
    else if ( result == GTK_RESPONSE_NO ) ; // 按No，沒反應，返回主視窗。 
  
    gtk_widget_destroy(dialog);
}

void button_clicked_callback(GtkWidget *button, GdkEventButton *event, gpointer window )
// 處理按鍵反應的主函式 
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
            if ( gButton[j][i] == button ) // 取得感應到的button的位置 
                over = TRUE ; 
    
    x = j-1 ; // (x,y)
    y = i-1 ; 

    if(event->type == GDK_BUTTON_PRESS && event->button == 3) { // 按下右鍵
        
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
    else if(event->type == GDK_BUTTON_PRESS && event->button == 2) // 按下中鍵
        showAgainQuestion( button, window ) ;   
    else if(event->type == GDK_BUTTON_PRESS && event->button == 1) { // 按下左鍵 
        buttonClicked( x, y ) ;
    
        for ( i = 0 ; i < HEIGHT ; i ++ )
            for ( j = 0 ; j < WIDTH ; j ++ )
                if ( getButtonState( j, i ) == hide )
                    gtk_widget_hide( gButton[j][i] ) ;
                    
        if ( isGameOver() ) { // 如果按到炸彈就會跳出問題視窗 
            g_string_append( str, utf8("掃雷失敗，光榮炸死！") );
            g_string_append( str, utf8(" ( 遊戲時間: ") );
            g_string_append( str, getLabelOfTimeStr() );
            g_string_append( str, utf8(" 秒 ) ") );
            showQuitQuestion( button, window, str->str, "flag.png" ) ; 
        }
        else if ( isAllBombOK() ) { // 是否掃雷完成  
            g_string_append( str, utf8("恭喜你掃雷完成！！！") );
            g_string_append( str, utf8(" ( 遊戲時間: ") ) ;
            g_string_append( str, getLabelOfTimeStr() );
            g_string_append( str, utf8(" 秒 ) ") );
            showQuitQuestion( button, window, str->str, "leaf.gif" ) ;
        }
    }    
}

const gchar *getColor( int num )
// 回傳應該使用的顏色 
{
    if ( num == 0 )
        return "<span foreground=\"#333333\">" ; // 黑色 
    else if ( num == 1 )
        return "<span foreground=\"#3333FF\">" ; // 藍色
    else if ( num == 2 )
        return "<span foreground=\"#009900\">" ; // 綠色 
    else if ( num == 3 )
        return "<span foreground=\"#FF0000\">" ; // 紅色 
    else if ( num == 4 )
        return "<span foreground=\"#6600FF\">" ; // 紫色 
    else if ( num == 5 )
        return "<span foreground=\"#996600\">" ; // 橙色 
    else if ( num == 6 )
        return "<span foreground=\"#999900\">" ; // 黃色 
    else if ( num == 7 )
        return "<span foreground=\"#00FFCC\">" ; // 青色 
    else
        return "<span foreground=\"#333333\">" ; // 黑色 
}

void reset( GtkWidget *window, int countOfBomb, int height, int width )
// 重新設定遊戲，全部還原
{
    GtkWidget *table;
    GtkWidget *label;
    int i = 0, j = 0, count = 0 ;
    GString *numOfStr = g_string_new("");
    
    srand(time(NULL)) ; // 重設亂數 
    
    setGameOver( FALSE ) ; // 重新開始遊戲 
    setLabelOfTimeStr( "0" ) ; // 遊戲時間歸零 
    setAllBomb( countOfBomb ) ; // 設置炸彈 
    
    for ( i = 0 ; i < height ; i ++ ) {
        for ( j = 0 ; j < width ; j ++ ) { 
            gtk_button_set_label( GTK_BUTTON( gButton[j][i] ), "" ) ; // 把上面的*號拿掉 
            
            setButtonState( j, i, show ) ; // 全部按鈕都顯示出來 
            count = countOfSurroundingsBomb( j, i ) ; // 周圍炸彈數加總 
            setBombNumber( j, i, count ) ; // 寫入全域變數 
            g_string_printf( numOfStr, "%d", count); // 將數字轉為字串存入numOfStr 
            
            if ( isBomb(j , i ) )
                 // 給炸彈本身標上記號(上面蓋上按鈕，所以看不到) 
                g_string_append(numOfStr, "(@)");  
            
            
            g_string_prepend( numOfStr, getColor( count ) ) ; // 取得該有的顏色 
            g_string_append( numOfStr, "</span>" ) ;
            gtk_label_set_markup(GTK_LABEL(getLabel( j, i )), numOfStr->str ); // 放上顏色 

        }
    }   
    g_string_printf( numOfStr, "%d", countOfBomb );
    setLabelOfBombStr( numOfStr->str ) ; // 還原顯示的炸彈數 
    
    gtk_widget_show_all( window ) ; // 按鈕全部還原 
} 

void timeout_callback( GtkWidget *label )
// 每一段時間都更改label 
{
    GString *temp = g_string_new( gtk_label_get_text( GTK_LABEL( label ) ) ) ;
    
    int timeNow = atoi( temp->str ) + 1 ;
    
    g_string_printf( temp, "%d", timeNow );
    gtk_label_set_text( GTK_LABEL( label ), temp->str ) ;  
}

gboolean combo_changed(GtkComboBox *comboBox, gpointer window ) 
// 因應選取的炸彈數做更動 
{
    gchar *active = gtk_combo_box_get_active_text(comboBox) ;
    int count = 0 ;
    
    count =  atoi( active ) ; // 將所選的炸彈數紀錄起來 
    setDefineCountOfBomb( count ) ;
    
    reset( window, count, HEIGHT, WIDTH ) ; // 重新設定遊戲 

    // gtk_combo_box_set_active( comboBox, 0 ); // 依然顯示第一個 
}


GtkWidget *setComboBox()
// 設置comboBox，可直接修改炸彈數目  
{
    GtkWidget *comboBox = gtk_combo_box_new_text();
    gtk_combo_box_append_text(GTK_COMBO_BOX(comboBox), utf8( "重置地雷數" ));
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
    int countOfBomb = 0 ; // 周圍炸彈加總 

    GString *numOfStr = g_string_new("");
    GString *str = g_string_new("");
    
    srand(time(NULL)) ; // 重設亂數 

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), utf8("踩地雷"));
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 300);
    table = gtk_table_new(HEIGHT, WIDTH, TRUE);
    
    setAllBomb( getDefineCountOfBomb() ) ; // 設置炸彈 
    
    for ( i = 0 ; i < HEIGHT ; i ++ ) {
        for ( j = 0 ; j < WIDTH ; j ++ ) { 
            setButtonState( j, i, show ) ; // 全部按鈕都顯示出來 
            countOfBomb = countOfSurroundingsBomb( j, i ) ; // 周圍炸彈數加總 
            setBombNumber( j, i, countOfBomb ) ; // 寫入全域變數 
            g_string_printf( numOfStr, "%d", countOfBomb); // 將數字轉為字串存入numOfStr 
            
            if ( isBomb(j , i ) )
                 // 給炸彈本身標上記號(上面蓋上按鈕，所以看不到) 
                g_string_append(numOfStr, "(@)");  
            
            label = gtk_label_new("") ;  
            
            g_string_prepend( numOfStr, getColor( countOfBomb ) ) ; // 取得該有的顏色 
            g_string_append( numOfStr, "</span>" ) ;
            gtk_label_set_markup(GTK_LABEL(label), numOfStr->str ); // 放上顏色
            
            setLabel( j, i, label ) ; // 將各個label存入全域變數gLabel
             
            gButton[j][i] = gtk_button_new_with_label( "" ) ; 
            
            gtk_table_attach_defaults( // 將button放入table 
                GTK_TABLE(table), gButton[j][i], j, j + 1, i, i + 1);
            gtk_table_attach_defaults( // 將label放入table 
                GTK_TABLE(table), label, j, j + 1, i, i + 1);  
        }
    } 
    
    comboBox = setComboBox() ; // 設置comboBox，可直接修改炸彈數目 
    
    labelOfBombStr = gtk_label_new(NULL);
    
    gtk_label_set_markup(GTK_LABEL(labelOfBombStr), \
        utf8( "<span foreground=\"#336699\">地雷數 :</span>" ) ); // 設置label字體顏色 
    
    g_string_printf( str, "%d", getDefineCountOfBomb() );
    
    labelOfTimeStr = gtk_label_new( "" ) ;
    gtk_label_set_markup(GTK_LABEL(labelOfTimeStr), \
        utf8( "<span foreground=\"#336699\">     已使用時間 :</span>" ) );
    labelOfTimeStr2 = gtk_label_new( utf8( "秒   " ) ) ;
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
                                
    g_signal_connect(GTK_OBJECT(comboBox), "changed", // 對應comboBox 
                     G_CALLBACK(combo_changed), window );
                                
    g_signal_connect(GTK_OBJECT(window), "destroy",
                     G_CALLBACK(gtk_main_quit), NULL);
    
    g_timeout_add(1000, (GSourceFunc) timeout_callback, labelOfTime ); // 計算遊戲時間 

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
  
