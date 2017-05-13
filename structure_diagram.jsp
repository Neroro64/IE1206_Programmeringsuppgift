JSP Diagram
   �   +         �    
TemperatureSpeaker.c����                  
   D .
Set up ports
define global variables
ADinit()                   D 

main loop                  
   � 
forever                    �   � %
PORTA.3 == 0?
is the button pressed?                       r   � 
...                �   � c
read the value from AD-converter
and calculate the current temperature

temp = current temperature               �   � #
store sound sequences
in message[]                   � 
loops two times               	     �  � 
yes         
   
     �   
d = first digit in temp   	            �  � 
no                 V   
d = second digit in temp                 � 
is this the first loop?         	           �  � 
inner for-loop                   Q 
Outer for-loop                 �  � 
forever                       /
ch = returnDigitAddresses(d)
message[i++] = ch               �   1
ch == 0xFF?
is it the end of the sound sequence?                    !  S 
yes                   � 
break               Z  S 
no                 Z  � 
...               �   � 
is pic-talker talking?                    �  Q 
yes                 �  � 
...               %  Q 
no                   � 
mode = 1
start talking               <  Y 
interrupt on TIMER2����         *        2  � 
mode?                !   "   #   $        �  � 
0                 �  � 
wait for talk                @  � 
1         %   !     �  � 
2         &   "     r  � 
3         '   #       � 
4         (   $     �  � 
5         )   %     �  � 
get next phoneme or pause           &     �  � 
initiate for pause   !       '     B  � 
initiate for phoneme   "       (     �  � 
go on with the pause   #       )     v  � 
go on with phoneme   $       *       � 
update PWM                V  �   �    