#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <graphics.h>
#include <process.h>
#include <stdlib.h>

//FUNCTION DEFINATION
int initmouse ( void ) ;
void showmouseptr ( void ) ;
void restrictmouseptr ( int , int , int , int ) ;
void getmousepos ( int * , int * , int * ) ;
void hidemouseptr ( ) ;
void displaymenu ( char ** , int , int ,int ) ;
void savemenu ( char ** , char ** , int , int , int , int ) ;
int getresponse ( char ** , int , int , int , int ) ;
void first_screen ( ) ;
void second_screen ( ) ;
void third_screen ( ) ;
int mouse_code ( char ** , int , int , int ) ;

//GLOBAL VARIABLES
char * menu1 [ ] = { "ENTER" , "EXIT" } ;
char * menu2 [ ] = { "DEVICE 1" , "DEVICE 2" , "DEVICE 3" , "DEVICE 4" , "BACK" } ;
char * menu3 [ ] = { "ON" , "OFF" , "STATUS" , "BACK" } ;
int state [ 4 ] [ 2 ] , value = 1 ;     //1ST COL. -> DEVICE, 2ND COL. -> ON
union REGS i , o ;

void main ( )
{
	int gd = DETECT , gm , choice , width , maxx , maxy , i , j ;

	initgraph ( &gd , &gm , "C:\\tc\\bgi" ) ;

	for ( i = 0 ; i < 4 ; i++ )
		for ( j = 0 ; j < 2 ; j++ )
			state [ i ] [ j ] = 0 ;

	maxx = getmaxx ( ) ;
	maxy = getmaxy ( ) ;
	setviewport ( 1 , 1 , maxx - 1 , maxy - 1 , 1 ) ;

	if ( initmouse ( ) == 0 )
	{
		closegraph ( ) ;
		restorecrtmode ( ) ;
		printf ( "\nMouse Driver Not Loaded" ) ;
		exit ( 1 ) ;
	}

	restrictmouseptr ( 1 , 1 , maxx - 1 , maxy - 1 ) ;
	showmouseptr ( ) ;

	first_screen ( ) ;
	//second_screen ( ) ;
	//third_screen ( ) ;

	getch ( ) ;
}

//INITIALIZE MOUSE
int initmouse ( )
{
	i.x.ax = 0 ;
	int86 ( 0x33 , &i , &o ) ;
	return ( o.x.ax ) ;
}

//DISPLAY MOUSE POINTER
void showmouseptr ( )
{
	i.x.ax = 1 ;
	int86 ( 0x33 , &i , &o ) ;
}

//HIDE MOUSE POINTER
void hidemouseptr ( )
{
	i.x.ax = 2 ;
	int86 ( 0x33 , &i , &o ) ;
}

//RESTRICT MOUSE MOVEMENT
void restrictmouseptr ( int x1 , int y1 , int x2 , int y2 )
{
	i.x.ax = 7 ;
	i.x.cx = x1 ;
	i.x.dx = x2 ;
	int86 ( 0x33 , &i , &o ) ;
	i.x.ax = 8 ;
	i.x.cx = y1 ;
	i.x.dx = y2 ;
	int86 ( 0x33 , &i , &o ) ;
}

//GETS MOUSE COORDINATES AND BUTTON STATUS
void getmousepos ( int * button , int * x , int * y )
{
	i.x.ax = 3 ;
	int86 ( 0x33 , &i , &o ) ;
	* button = o.x.bx ;
	* x = o.x.cx ;
	* y = o.x.dx ;
}

//DISPALY MENU
void displaymenu ( char ** menu , int count , int x1 , int y1 )
{
	int i , h ;

	setcolor ( 12 ) ;
	settextstyle ( TRIPLEX_FONT , 0 , 3 ) ;
	h = textheight ( menu [ 0 ] ) ;

	for ( i = 0 ; i < count ; i++ )
		outtextxy ( x1 , y1 + i * ( h + 20 ) , menu [ i ] ) ;
}

//SAVE MENU
void savemenu ( char ** menu , char ** buffer , int width , int count , int x1 , int y1 )
{
	int x2 , y2 , area , h , i ;

	h = textheight ( menu [ 0 ] ) ;
	x2 = x1 + width ;

	y2 = y1 + h ;
	area = imagesize ( x1 , y1 , x2 , y2 ) ;
	buffer [ 0 ] = ( char * ) malloc ( area ) ;
	getimage ( x1 , y1 , x2 , y2 , buffer [ 0 ] ) ;

	for ( i = 1 ; i < count ; i++ )
	{
		y1 = y1 + i * ( h + 20 ) ;
		y2 = y1 + i * h ;
		area = imagesize ( x1 , y1 , x2 , y2 ) ;
		buffer [ i ] = ( char * ) malloc ( area ) ;
		getimage ( x1 , y1 , x2 , y2 , buffer [ i ] ) ;
	}
}

int getresponse ( char ** menu , int count , int width , int x1 , int y1 )
{
	int choice = 1 , x , y , x2 , y2 , y3 , button , in , h , i ;

	h = textheight ( menu [ 0 ] ) ;
	x2 = x1 + width ;

	y2 = y1 + h ;
	setcolor ( 15 ) ;
	rectangle ( x1 - 4 , y1 , x2 + 4 , y2 + 4 ) ;

	for ( i = 1 ; i < count ; i++ )
	{
		y2 = y1 + i * ( h + 20 ) ;
		y3 = y2 + h ;
		rectangle ( x1 - 4 , y2 , x2 + 4 , y3 + 4 ) ;
	}

	while ( 1 )
	{
		getmousepos ( &button , &x , &y ) ;
		if ( x >= x1 && x <= x2 )
		{
			in = 1 ;

			for ( i = 0 ; i < count ; i++ )
			{
				y2 = y1 + i * ( h + 20 ) ;
				y3 = y2 + h ;

				if ( y >= y2 && y <= y3 )
					choice = i + 1 ;
			}

			if ( ( button & 1 ) == 1 )
			{
				while ( ( button & 1 ) == 1 )
					getmousepos ( &button , &x , &y ) ;

				for ( i = 0 ; i < count ; i++ )
				{
					y2 = y1 + i * ( h + 20 ) ;
					y3 = y2 + h ;

					if ( x >= x1 && x <= x2 && y >= y2 && y <= y3 )
						return ( choice ) ;
				}
			}
		}
		else
			if ( in == 1 )
				in = 0;
	}
}

void on ( int n )
{
	setcolor ( 12 ) ;
	outportb ( 0x378 , n ) ;
	delay ( 50 ) ;
	outportb ( 0x378 , 160 ) ;
	outtextxy ( 300 , 420 , "OK!" ) ;
	sound ( 2000 ) ;
	delay ( 300 ) ;
	nosound ( ) ;
}

void off ( int n )
{
	setcolor ( 12 ) ;
	outportb ( 0x378 , n ) ;
	delay ( 50 ) ;
	outportb ( 0x378 , 160 ) ;
	outtextxy ( 300 , 420 , "OK!" ) ;
	sound ( 2000 ) ;
	delay ( 300 ) ;
	nosound ( ) ;
}

void first_screen ( )
{
	int choice , maxy , count ;

	maxy = getmaxy ( ) ;

	setcolor ( 3 ) ;
	bar3d ( 65 , 130 , 565 , 300 , 5 , 5 ) ;

	settextstyle ( 1 , 0 , 3 ) ;
	setcolor ( 3 ) ;
	outtextxy ( 265 , 140 , "PROJECT" ) ;

	setcolor ( 0 ) ;
	outtextxy ( 90 , 170 , "INTERFACING PC WITH LIGHTS AND FANS" ) ;

	settextstyle ( 0 , 0 , 2 ) ;
	outtextxy ( 300 , 222 , "BY" ) ;

	outtextxy ( 90 , 240 , "Lalit Gupta and Piyush Kansal" ) ;

	sound ( 5000 ) ;
	delay ( 300 ) ;
	nosound ( ) ;

	count = sizeof ( menu1 ) / sizeof ( char * ) ;
	choice = mouse_code ( menu1 , count , 280 , 340 ) ;

	if ( choice == 1 )
	{
		clearviewport ( ) ;
		second_screen ( ) ;
	}

	if ( choice == 2 )
	{
		clearviewport ( ) ;
		hidemouseptr ( ) ;
		settextstyle ( 1 , 0 , 4 ) ;
		setcolor ( 3 ) ;
		outtextxy ( 30 , maxy / 2 - 50 , "THANKYOU FOR USING THIS PROJECT" ) ;
		delay ( 3000 ) ;
	}

	exit ( 0 ) ;

}

void second_screen ( )
{
	int choice , count ;

	setcolor ( 1 ) ;
	settextstyle ( 1 , 0 , 4 ) ;
	outtextxy ( 150 , 100 , "SELECT YOUR DEVICE :" ) ;

	count = sizeof ( menu2 ) / sizeof ( char * ) ;
	choice = mouse_code ( menu2 , count , 260 , 200 ) ;

	if ( choice == 1 || choice == 2 || choice == 3 || choice == 4 )
	{
		state [ choice - 1 ] [ 0 ] = value ;
		value++ ;
		clearviewport ( ) ;
		third_screen ( ) ;
	}

	if ( choice == 5 )
	{
		clearviewport ( ) ;
		first_screen ( ) ;
	}
}

void third_screen ( )
{
	int choice , count , state_variable , t , i ;

	setcolor ( 1 ) ;
	settextstyle ( 1 , 0 , 4 ) ;
	outtextxy ( 75 , 100 , "SELECT OPERATION ON DEVICE :" ) ;

	count = sizeof ( menu3 ) / sizeof ( char * ) ;
	choice = mouse_code ( menu3 , count , 280 , 200 ) ;

	//FOR FINDING WHICH LATEST DEVICE HAS BEEN SELECTED
	state_variable = state [ 0 ] [ 0 ] ;
	t = 0 ;
	for ( i = 0 ; i < 3 ; i++ )
	{
		if ( state_variable > state [ i + 1 ] [ 0 ] )
			continue ;
		else
		{
			state_variable = state [ i + 1 ] [ 0 ] ;
			t = i + 1 ;
		}
	}

	switch ( t + 1 )
		{
			case 1 :
				switch ( choice )
				{
					case 1 :
						on ( 98 ) ;
						state [ 0 ] [ 1 ] = 1 ;
						break ;

					case 2 :
						off ( 34 ) ;
						state [ 0 ] [ 1 ] = 0 ;
						break ;

					case 3 :
							if ( state [ 0 ] [ 1 ] == 0 )
							{
								outtextxy ( 220 , 400 , "DEVICE 1 IS OFF" ) ;
								sound ( 2000 ) ;
								delay ( 300 ) ;
								nosound ( ) ;
							}
							else
							{
								outtextxy ( 220 , 300 , "DEVICE 1 IS ON" ) ;
								sound ( 2000 ) ;
								delay ( 300 ) ;
								nosound ( ) ;
							}
							break;

					case 4 :
						second_screen ( ) ;
						break ;
				}
				break ;

			case 2 :
				switch ( choice )
				{
					case 1 :
						on ( 100 ) ;
						state [ 1 ] [ 1 ] = 1 ;
						break ;

					case 2 :
						off ( 36 ) ;
						state [ 1 ] [ 1 ] = 0 ;
						break ;

					case 3 :
						if ( state [ 1 ] [ 1 ] == 0 )
						{
							outtextxy ( 220 , 400 , "DEVICE 2 IS OFF" ) ;
							sound ( 2000 ) ;
							delay ( 300 ) ;
							nosound ( ) ;
						}
						else
						{
							outtextxy ( 220 , 300 , "DEVICE 2 IS ON" ) ;
							sound ( 2000 ) ;
							delay ( 300 ) ;
							nosound ( ) ;
						}
						break;

					case 4 :
						second_screen ( ) ;
						break ;
				}
				break ;

			case 3 :
					switch ( choice )
					{
						case 1 :
							on ( 104 ) ;
							state [ 2 ] [ 1 ] = 1 ;
							break ;

						case 2 :
							off ( 40 ) ;
							state [ 2 ] [ 1 ] = 0 ;
							break ;

						case 3 :
							if ( state [ 2 ] [ 1 ] == 0 )
							{
								outtextxy ( 220 , 400 , "DEVICE 3 IS OFF" ) ;
								sound ( 2000 ) ;
								delay ( 300 ) ;
								nosound ( ) ;
							}
							else
							{
								outtextxy ( 220 , 300 , "DEVICE 3 IS ON" ) ;
								sound ( 2000 ) ;
								delay ( 300 ) ;
								nosound ( ) ;
							}
							break;

						case 4 :
							second_screen ( ) ;
							break ;
					}
					break ;

			case 4 :
				switch ( choice )
				{
					case 1 :
						on ( 112 ) ;
						state [ 3 ] [ 1 ] = 1 ;
						break ;

					case 2 :
						off ( 48 ) ;
						state [ 3 ] [ 1 ] = 0 ;
						break ;

					case 3 :
						if ( state [ 3 ] [ 1 ] == 0 )
						{
							outtextxy ( 220 , 400 , "DEVICE 4 IS OFF" ) ;
							sound ( 2000 ) ;
							delay ( 300 ) ;
							nosound ( ) ;
						}
						else
						{
							outtextxy ( 220 , 300 , "DEVICE 4 IS ON" ) ;
							sound ( 2000 ) ;
							delay ( 300 ) ;
							nosound ( ) ;
						}
						break;

					case 4 :
						second_screen ( ) ;
						break ;
				}
				break ;

		}

	delay ( 3000 ) ;
	clearviewport ( ) ;
	second_screen ( ) ;
}

int mouse_code ( char ** menu , int count , int x , int y )
{
	char ** buffer ;
	int width = 0 , choice , i ;

	displaymenu ( menu , count , x , y ) ;

	for ( i = 0 ; i < count ; i++ )
	{
		if ( textwidth ( menu [ i ] ) > width )
			width = textwidth ( menu [ i ] ) ;
		else
			continue ;
	}

	buffer = ( char ** ) malloc ( sizeof ( menu ) ) ;
	savemenu ( menu , buffer , width , count , x , y ) ;

	choice = getresponse ( menu , count , width , x , y ) ;
	return choice ;
}