
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#define WIDTH (6)
float gamma = 0.80f;   // discount factor
float alpha = 0.02f;  // learning rate

float Q[1<<(WIDTH+WIDTH)];  // utility value
int   P[1<<(WIDTH+WIDTH)];  // counter (not really needed)
int height =0;
int explore=0;  // flag I used to experiment with exploration


unsigned result(unsigned state, unsigned action, unsigned piece) 
{
	piece <<= action;
	while((piece & state) || (piece <<WIDTH & state)) 
		piece <<=WIDTH;

	unsigned t = piece|state;

	if( (t&(((1<<WIDTH)-1)<<WIDTH)) == (((1<<WIDTH)-1)<<WIDTH)) 
		t= (t&((1<<WIDTH)-1)) | ((t>>(2*WIDTH))<<WIDTH);

	if( (t&((1<<WIDTH)-1)) == ((1<<WIDTH)-1)) 
		t>>=WIDTH;

	return t;
}

void init(){
	for(int i=0;i<(1<< 2*WIDTH);i++) 
	{
		Q[i] = 0.0f;
		P[i] = 0;
	}
}
unsigned rotate(unsigned p,int r) 
{
	while(r--) 
	{
		unsigned q = p>>WIDTH;
		p = ((p&1)?2:0) + ((p&2)?2<<WIDTH:0) +
		    ((q&2)?1<<WIDTH:0) + ((q&1)?1:0);
	}
	if(! (p%(1<<WIDTH))) 
		p >>=WIDTH;

	return p;
}


unsigned crank(unsigned state,unsigned piece)
{
	// find the best next state
	float best =-999999999.9f;
	unsigned t = 0;
	int loss=9999;
	for(int a=0;a<WIDTH-1;a++)
	{
	  for(int r=0;r<3;r++) 
	  {
		unsigned n = result(state,a,rotate(piece,r));
		int l=0;
		while(n>>(2*WIDTH)) 
		{
			n>>=WIDTH;
			l++;
		}
		assert(n<(1<< 2*WIDTH));
		// if((l<loss)||(l==loss && loss*-100 + gamma * Q[n] > best) ){
		if(l*-100 + gamma * Q[n] > best) 
		{
			t=n;
			loss = l;
			best = loss*-100 + gamma * Q[t];
		}
	  }
	}
	//if(loss)printf("Lost %d rows\n",loss);
	height += loss;
	// update Q[state] based on result from state s to t 
	Q[state] = (1-alpha) * Q[state] + alpha * best;
	
	// average learning rule:  (it was useless)
	//Q[state] = (P[state] * Q[state] + alpha * best)/(1+P[state]);
	P[state] ++;

	if(explore) 
	{
		loss=0;
		t = result(state,rand()%(WIDTH-1),rand()%4);
		while(t>>(2*WIDTH)) 
		{
			t>>=WIDTH;
		}
		//state=(state+1)%(1<<(2*WIDTH));
	}

	state = t;  // move to new state;
	return state;
}

int main(int,char**)
{
	init();
	int game=0;
	while(game<1<<13) 
	{
		//explore = (game%2);  // doing exploration didn't help learning
		srand(0);
		height=0;
		unsigned state =0;
		for(int i=0;i<10000;i++) 
		{
			unsigned piece = ((rand()%4)<<WIDTH) +  (rand()%3)+1;
			state = crank(state,piece);
		}
		game++;
		if(0==(game & (game-1)))  // if a power of 2
			printf("%4d %4d %s\n",game,height,(explore)?"learning":"");

	}
	return 0;
}
