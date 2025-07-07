#include <stdio.h>
#include <string.h>
#define _O 'O'
#define _E ' '
#define _WP 'P'
#define _WN 'N'
#define _WB 'B'
#define _WR 'R'
#define _WQ 'Q'
#define _WK 'K'
#define _BP 'p'
#define _BN 'n'
#define _BB 'b'
#define _BR 'r'
#define _BQ 'q'
#define _BK 'k'
char gB[120];char gS;char gE;char gC;char gK[2];
typedef unsigned int M_t;
#define M_F(m) ((m) & 0x7F)
#define M_T(m) (((m) >> 7) & 0x7F)
#define M_P(m) (((m) >> 14) & 0x7)
#define M_L(m) (((m) >> 17) & 0x1F)
#define M_V(f,t,p,l) ((M_t)((f)|((t)<<7)|((p)<<14)|((l)<<17)))
#define F_N 0x00
#define F_C 0x01
#define F_D 0x02
#define F_X 0x03
#define F_Y 0x04
#define F_Z 0x05
#define F_R 0x08
#define P_N 0
#define P_B 1
#define P_R 2
#define P_Q 3
M_t gL[256];int gM;
typedef struct{char e,c,x;}S_t;
S_t gT[256];int gI;
static const int dN[]={-21,-19,-12,-8,8,12,19,21};
static const int dK[]={-11,-10,-9,-1,1,9,10,11};
static const int dB[]={-11,-9,9,11};
static const int dR[]={-10,-1,1,10};
void fM(M_t m);void fU(M_t m);void fI();int fO(char c);int fP(char c);int fA(int q,int s);void fL(M_t m);void fP_pawn(int q);void fN_knight(int q);void fK_king(int q);void fR_sliding(int q,int*d,int c);void fG();char*fQ_sq(int q_sq);char*fS_move(M_t m);int fQ_str(char*s);M_t fP_parse(char*s);int fV(char p);int fS_score_move(M_t m);
int fO(char c){return(c==_E||c==_O)?0:(gS==0?(c>='A'&&c<='Z'):(c>='a'&&c<='z'));}
int fP(char c){return(c==_E||c==_O)?0:(gS==0?(c>='a'&&c<='z'):(c>='A'&&c<='Z'));}
void fL(M_t m){fM(m);if(!fA(gK[!gS],gS))gL[gM++]=m;fU(m);}
void fI(){int i,j;for(i=0;i<120;i++)gB[i]=_O;for(i=0;i<8;i++)for(j=0;j<8;j++)gB[21+i*10+j]=_E;gB[21]=_WR;gB[22]=_WN;gB[23]=_WB;gB[24]=_WQ;gB[25]=_WK;gB[26]=_WB;gB[27]=_WN;gB[28]=_WR;for(i=0;i<8;i++)gB[31+i]=_WP;for(i=0;i<8;i++)gB[81+i]=_BP;gB[91]=_BR;gB[92]=_BN;gB[93]=_BB;gB[94]=_BQ;gB[95]=_BK;gB[96]=_BB;gB[97]=_BN;gB[98]=_BR;gS=0;gE=0;gC=15;gK[0]=25;gK[1]=95;gI=0;}
void fM(M_t m_val){char f=M_F(m_val),t=M_T(m_val),l=M_L(m_val),p_prom_type=M_P(m_val);gT[gI].e=gE;gT[gI].c=gC;gT[gI].x=_E;gI++;gE=0;if(gB[f]==_WK)gC&=~3;else if(gB[f]==_BK)gC&=~12;if(f==21||t==21)gC&=~2;if(f==28||t==28)gC&=~1;if(f==91||t==91)gC&=~8;if(f==98||t==98)gC&=~4;if(gB[t]!=_E)gT[gI-1].x=gB[t];gB[t]=gB[f];gB[f]=_E;if(gB[t]==_WK)gK[0]=t;else if(gB[t]==_BK)gK[1]=t;switch(l){case F_D:gE=(gS==0)?(t-10):(t+10);break;case F_X:gT[gI-1].x=(gS==0)?_BP:_WP;gB[(gS==0)?(t+10):(t-10)]=_E;break;case F_Y:gB[(gS==0)?28:98]=_E;gB[(gS==0)?26:96]=(gS==0)?_WR:_BR;break;case F_Z:gB[(gS==0)?21:91]=_E;gB[(gS==0)?24:94]=(gS==0)?_WR:_BR;break;}if(l&F_R)gB[t]=(gS==0)?(p_prom_type==P_N?_WN:(p_prom_type==P_B?_WB:(p_prom_type==P_R?_WR:_WQ))):(p_prom_type==P_N?_BN:(p_prom_type==P_B?_BB:(p_prom_type==P_R?_BR:_BQ)));gS=!gS;}
void fU(M_t m_val){gI--;char f=M_F(m_val),t=M_T(m_val),l=M_L(m_val);gB[f]=(l&F_R)?((gS==0)?_BP:_WP):gB[t];gB[t]=gT[gI].x;switch(l){case F_X:gB[(gS==0)?(t+10):(t-10)]=(gS==0)?_WP:_BP;break;case F_Y:gB[(gS==1)?28:98]=((gS==1)?_WR:_BR);gB[(gS==1)?26:96]=_E;break;case F_Z:gB[(gS==1)?21:91]=((gS==1)?_WR:_BR);gB[(gS==1)?24:94]=_E;break;}gE=gT[gI].e;gC=gT[gI].c;gS=!gS;if(gB[f]==_WK)gK[0]=f;else if(gB[f]==_BK)gK[1]=f;}
int fA(int q_sq,int s_attacker_side){char c;int t,i,d;char ap=(s_attacker_side==0)?_WP:_BP,an=(s_attacker_side==0)?_WN:_BN,ak=(s_attacker_side==0)?_WK:_BK,ar=(s_attacker_side==0)?_WR:_BR,ab=(s_attacker_side==0)?_WB:_BB,aq=(s_attacker_side==0)?_WQ:_BQ;if(s_attacker_side==0){if(gB[q_sq-11]==ap||gB[q_sq-9]==ap)return 1;}else{if(gB[q_sq+11]==ap||gB[q_sq+9]==ap)return 1;}for(i=0;i<8;i++){t=q_sq+dN[i];if(gB[t]==an)return 1;}for(i=0;i<8;i++){t=q_sq+dK[i];if(gB[t]==ak)return 1;}for(i=0;i<4;i++){d=dR[i];t=q_sq+d;while(gB[t]!=_O){c=gB[t];if(c!=_E){if(c==ar||c==aq)return 1;break;}t+=d;}}for(i=0;i<4;i++){d=dB[i];t=q_sq+d;while(gB[t]!=_O){c=gB[t];if(c!=_E){if(c==ab||c==aq)return 1;break;}t+=d;}}return 0;}
void fP_pawn(int q_sq){int t,pd,dd,cl,cr;if(gS==0){pd=10;dd=20;cl=9;cr=11;t=q_sq+pd;if(gB[t]==_E){if(t>=91&&t<=98){fL(M_V(q_sq,t,P_Q,F_R));fL(M_V(q_sq,t,P_R,F_R));fL(M_V(q_sq,t,P_B,F_R));fL(M_V(q_sq,t,P_N,F_R));}else fL(M_V(q_sq,t,0,F_N));if(q_sq>=31&&q_sq<=38){t=q_sq+dd;if(gB[q_sq+pd]==_E&&gB[t]==_E)fL(M_V(q_sq,t,0,F_D));}}t=q_sq+cl;if(gB[t]!=_O&&fP(gB[t])){if(t>=91&&t<=98){fL(M_V(q_sq,t,P_Q,F_R|F_C));fL(M_V(q_sq,t,P_R,F_R|F_C));fL(M_V(q_sq,t,P_B,F_R|F_C));fL(M_V(q_sq,t,P_N,F_R|F_C));}else fL(M_V(q_sq,t,0,F_C));}t=q_sq+cr;if(gB[t]!=_O&&fP(gB[t])){if(t>=91&&t<=98){fL(M_V(q_sq,t,P_Q,F_R|F_C));fL(M_V(q_sq,t,P_R,F_R|F_C));fL(M_V(q_sq,t,P_B,F_R|F_C));fL(M_V(q_sq,t,P_N,F_R|F_C));}else fL(M_V(q_sq,t,0,F_C));}if(gE!=0){if(q_sq+cl==gE)fL(M_V(q_sq,gE,0,F_X));if(q_sq+cr==gE)fL(M_V(q_sq,gE,0,F_X));}}else{pd=-10;dd=-20;cl=-9;cr=-11;t=q_sq+pd;if(gB[t]==_E){if(t>=21&&t<=28){fL(M_V(q_sq,t,P_Q,F_R));fL(M_V(q_sq,t,P_R,F_R));fL(M_V(q_sq,t,P_B,F_R));fL(M_V(q_sq,t,P_N,F_R));}else fL(M_V(q_sq,t,0,F_N));if(q_sq>=81&&q_sq<=88){t=q_sq+dd;if(gB[q_sq+pd]==_E&&gB[t]==_E)fL(M_V(q_sq,t,0,F_D));}}t=q_sq+cl;if(gB[t]!=_O&&fP(gB[t])){if(t>=21&&t<=28){fL(M_V(q_sq,t,P_Q,F_R|F_C));fL(M_V(q_sq,t,P_R,F_R|F_C));fL(M_V(q_sq,t,P_B,F_R|F_C));fL(M_V(q_sq,t,P_N,F_R|F_C));}else fL(M_V(q_sq,t,0,F_C));}t=q_sq+cr;if(gB[t]!=_O&&fP(gB[t])){if(t>=21&&t<=28){fL(M_V(q_sq,t,P_Q,F_R|F_C));fL(M_V(q_sq,t,P_R,F_R|F_C));fL(M_V(q_sq,t,P_B,F_R|F_C));fL(M_V(q_sq,t,P_N,F_R|F_C));}else fL(M_V(q_sq,t,0,F_C));}if(gE!=0){if(q_sq+cl==gE)fL(M_V(q_sq,gE,0,F_X));if(q_sq+cr==gE)fL(M_V(q_sq,gE,0,F_X));}}}
void fN_knight(int q_sq){int i,t;for(i=0;i<8;i++){t=q_sq+dN[i];if(gB[t]==_O)continue;if(fO(gB[t]))continue;if(gB[t]==_E)fL(M_V(q_sq,t,0,F_N));else fL(M_V(q_sq,t,0,F_C));}}
void fK_king(int q_sq){int i,t;for(i=0;i<8;i++){t=q_sq+dK[i];if(gB[t]==_O)continue;if(fO(gB[t]))continue;if(gB[t]==_E)fL(M_V(q_sq,t,0,F_N));else fL(M_V(q_sq,t,0,F_C));}if(gS==0){if(gC&1){if(gB[26]==_E&&gB[27]==_E){if(!fA(25,1)&&!fA(26,1)&&!fA(27,1))fL(M_V(25,27,0,F_Y));}}if(gC&2){if(gB[22]==_E&&gB[23]==_E&&gB[24]==_E){if(!fA(25,1)&&!fA(24,1)&&!fA(23,1))fL(M_V(25,23,0,F_Z));}}}else{if(gC&4){if(gB[96]==_E&&gB[97]==_E){if(!fA(95,0)&&!fA(96,0)&&!fA(97,0))fL(M_V(95,97,0,F_Y));}}if(gC&8){if(gB[92]==_E&&gB[93]==_E&&gB[94]==_E){if(!fA(95,0)&&!fA(94,0)&&!fA(93,0))fL(M_V(95,93,0,F_Z));}}}}
void fR_sliding(int q_sq,int*d_dirs,int c_num_dirs){int i,D,t;for(i=0;i<c_num_dirs;i++){D=d_dirs[i];t=q_sq+D;while(gB[t]!=_O){if(fO(gB[t]))break;if(gB[t]==_E)fL(M_V(q_sq,t,0,F_N));else{fL(M_V(q_sq,t,0,F_C));break;}t+=D;}}}
void fG(){gM=0;int q;char c;for(q=21;q<=98;q++){c=gB[q];if(c==_O||c==_E)continue;if(gS==0){if(c==_WP)fP_pawn(q);else if(c==_WN)fN_knight(q);else if(c==_WB)fR_sliding(q,(int*)dB,4);else if(c==_WR)fR_sliding(q,(int*)dR,4);else if(c==_WQ){fR_sliding(q,(int*)dR,4);fR_sliding(q,(int*)dB,4);}else if(c==_WK)fK_king(q);}else{if(c==_BP)fP_pawn(q);else if(c==_BN)fN_knight(q);else if(c==_BB)fR_sliding(q,(int*)dB,4);else if(c==_BR)fR_sliding(q,(int*)dR,4);else if(c==_BQ){fR_sliding(q,(int*)dR,4);fR_sliding(q,(int*)dB,4);}else if(c==_BK)fK_king(q);}}}
char*fQ_sq(int q_sq){static char s[3];s[0]=(q_sq%10)-1+'a';s[1]=(q_sq/10)-2+'1';s[2]='\0';return s;}
char*fS_move(M_t m_val){static char s[6];char fs[3],ts[3],pc='\0';strcpy(fs,fQ_sq(M_F(m_val)));strcpy(ts,fQ_sq(M_T(m_val)));if(M_L(m_val)&F_R){char p_chars[]={'n','b','r','q'};pc=p_chars[M_P(m_val)];}s[0]=fs[0];s[1]=fs[1];s[2]=ts[0];s[3]=ts[1];s[4]=pc;s[5]='\0';return s;}
int fQ_str(char*s_str){return(s_str[1]-'1'+2)*10+(s_str[0]-'a'+1);}
M_t fP_parse(char*s_str){int len=strlen(s_str);int f=fQ_str(s_str),t=fQ_str(s_str+2),pt=0;char pc='\0';if(len==5){pc=s_str[4];switch(pc){case'n':pt=P_N;break;case'b':pt=P_B;break;case'r':pt=P_R;break;case'q':pt=P_Q;break;default:return 0;}}else if(len!=4)return 0;fG();for(int i=0;i<gM;i++){M_t cm=gL[i];if(M_F(cm)==f&&M_T(cm)==t){if((M_L(cm)&F_R)&&M_P(cm)==pt)return cm;if(!(M_L(cm)&F_R)&&pc=='\0')return cm;}}return 0;}
int fV(char p){if(p==_WP||p==_BP)return 100;if(p==_WN||p==_BN)return 300;if(p==_WB||p==_BB)return 300;if(p==_WR||p==_BR)return 500;if(p==_WQ||p==_BQ)return 900;return 0;}
int fS_score_move(M_t m){int score=0;char moving_piece=gB[M_F(m)];char captured_piece=gB[M_T(m)];if(M_L(m)&F_R)score+=900;if(captured_piece!=_E)score+=10*fV(captured_piece)-fV(moving_piece);else if(M_L(m)==F_X)score+=10*fV((gS==0)?_BP:_WP)-fV((gS==0)?_WP:_BP);return score;}

int main(){char l[9999];setbuf(stdin,NULL);setbuf(stdout,NULL);fI();
while(fgets(l,sizeof(l),stdin)){if(strncmp(l,"uci",3)==0){printf("uciok\n");}
else if(strncmp(l,"isready",7)==0){printf("readyok\n");}
else if(strncmp(l,"position startpos",17)==0){fI();char*ms=strstr(l,"moves");if(ms){ms+=6;char*tk=strtok(ms," \n");while(tk){M_t mv=fP_parse(tk);if(mv!=0)fM(mv);tk=strtok(NULL," \n");}}}
else if(strncmp(l,"go",2)==0){fG();if(gM>0){for(int i=0;i<gM-1;i++){for(int j=i+1;j<gM;j++){if(fS_score_move(gL[j])>fS_score_move(gL[i])){M_t temp=gL[i];gL[i]=gL[j];gL[j]=temp;}}}printf("info score cp 0\n");printf("bestmove %s\n",fS_move(gL[0]));}else printf("bestmove (none)\n");}
else if(strncmp(l,"quit",4)==0)break;}return 0;}
