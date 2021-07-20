#include <stdio.h>
#include <stdlib.h>

// derived from findAverages function, which is not used at runtime
const float averageScores[7] = {0, 25, 50, 86.806, 143.519, 225.084, 932.041};
const float scoreFactors[7] = {0, 0.333333, 0.555556, 0.722222, 0.842593, 0.922840, 0.976852};

// score 1s and 5s in a roll
int scorePointDice(int* roll, int len, int dice){
	int score = 0;
	int d = 0;

	for(int i = 0; i < len; i++){
		if(d >= dice) return score;
		if(roll[i] == 1){
			score += 100;
			d++;
		}
	}
	for(int i = 0; i < len; i++){
		if(d >= dice) return score;
		if(roll[i] == 5){
			score += 50;
			d++;
		}
	}
	return score;
}

// score ones and fives given a frequency array
int scorePDFreq(int* freq, int dice){
	int score = 0;
	int d = 0;
	for(int i = 0; i < dice; i++){
		if(d >= dice) return score;
		if(freq[0] == 0) break;
		score += 100;
		freq[0]--;
		d++;
	}
	for(int i = 0; i < dice - d; i++){
		if(d >= dice) return score;
		if(freq[4] == 0) break;
		score += 50;
		freq[0]--;
		d++;
	}
	
	return score;
}

// score combinations of x amount of n dice
int scoreOfAKind(int n, int x){
	if(x < 3) return 0;
	int s = n == 1 ? 1000 : n * 100;
	s *= (x - 2);
	return s;
}

// score the sets (555, 3333, etc.) in a roll + 1s and 5s
int scoreSets(int* freq, int dice){
	int score = 0;

	for(int i = 0; i < 6; i++){
		for(int j = dice; j >=3; j--){
			if(freq[i] >= j){
				int set = scoreOfAKind(i + 1, j);
				freq[i] -= j;
				set += scorePDFreq(freq, dice - j);
				if(set > score) score = set;
			}
		}
	}

	return score;
}

// max score of a roll when keeping [dice] dice
int scoreRoll(int* roll, int len, int dice){
	int score = 0;

	if(dice <= 2){
		return scorePointDice(roll, len, dice);
	}else if(dice == 3){
		int freq[6] = {0, 0, 0, 0, 0, 0};
		for(int i = 0; i < len; i++){
			freq[roll[i] - 1]++;
		}

		score = scoreSets(freq, dice);
		if(score > 200) return score;
		else{
			int pd = scorePointDice(roll, len, dice);
			return pd > score ? pd : score;
		}
	}else if(dice == 4){
		int freq[6] = {0, 0, 0, 0, 0, 0};
		for(int i = 0; i < len; i++){
			freq[roll[i] - 1]++;
		}

		score = scoreSets(freq, dice);

		int pd = scorePointDice(roll, len, dice);
		return pd > score ? pd : score;
	}else if(dice == 5){
		int freq[6] = {0, 0, 0, 0, 0, 0};
		for(int i = 0; i < len; i++){
			freq[roll[i] - 1]++;
		}

		score = scoreSets(freq, dice);

		int pd = scorePointDice(roll, len, dice);
		return pd > score ? pd : score;
	}if(dice == 6){
		int freq[6] = {0, 0, 0, 0, 0, 0};
		for(int i = 0; i < len; i++){
			freq[roll[i] - 1]++;
		}

		if(freq[0] == 1 && freq[0] == freq[1] && freq[1] == freq[2] &&
			freq[2] == freq[3] && freq[3] == freq[4] && freq[4] == freq[5]){
			return 2500;
		}
		
		int trips = 0;
		int pairs = 0;
		for(int i = 0; i < 6; i++){
			if(freq[i] == 6){
				return 10000;
			}else if(freq[i] >= 5){
				int quin = scoreOfAKind(i + 1, 5);
				freq[i] -= 5;
				quin += scorePDFreq(freq, 1);
				if(quin > score) score = quin;
			}if(freq[i] >= 4){
				int quad = scoreOfAKind(i + 1, 4);
				freq[i] -= 4;
				quad += scorePDFreq(freq, 2);
				if(quad > score) score = quad;
				pairs+=2;
			}else if(freq[i] >= 3){
				int trip = scoreOfAKind(i + 1, 3);
				freq[i] -= 3;
				trip += scorePDFreq(freq, 3);
				if(trip > score) score = trip;
				trips++;
			}else if(freq[i] >= 2) pairs++;
		}
		if(pairs == 3) return 1500;
		if(trips == 2) return 2500;

		int pd = scorePointDice(roll, len, dice);
		return pd > score ? pd : score;
	}
}

int powInt(int base, int exp){
	int p = base;
	for(int i = 1; i < exp; i++){
		p *= base;
	}
	return p;
}

void findAverages(float* scrAv, float* scrFac, int dice){
	int perms = powInt(6, dice);
	int roll[dice];
	int totalScore = 0;
	int scoringRolls = 0;

	for(int i = 0; i < dice; i++){
		roll[i] = 6;
	}
	
	for(int i = 0; i < perms; i++){
		int s = scoreRoll(roll, dice, dice);
		if(s > 0){
			totalScore += s;
			scoringRolls++;
		}

		for(int p = dice - 1; p >= 0; p--){
			if(roll[p] > 1){
				roll[p]--;
				for(int r = p + 1; r < dice; r++){
					roll[r] = 6;
				}
				break;
			}
		}
	}
	float p = perms;
	float t = totalScore;
	float s = scoringRolls;

	*scrAv = t / p;
	*scrFac = s / p;
}

void calcDecision(int* reroll, int* keepScore, int* roll, int len, int score){
	int last = 0;
	int ks = 0;
	int rrl = 0;
	int scr;
	float best = 0;

	for(int i = 1; i <= len; i++){
		scr = scoreRoll(roll, len, i);

		if(scr > last){
			int n = i == len ? len : len - i;
			float rrlScr = (score + scr + averageScores[n]) * scoreFactors[n];
			if(rrlScr > best){
				best = rrlScr;
				rrl = n;
				ks = scr;
			}
		}

		last = scr;
	}

	if(best < score){
		rrl = 0;
		ks = score + scr;
	}

	*reroll = rrl;
	*keepScore = score + ks;
}

void main(){
	int reroll, keepScore;
	int prevScore;

	char buf[6];
	int len = 0;

	printf("Enter Roll: ");
	scanf("%s", buf);
	printf("Enter score this turn: ");
	scanf("%d", &prevScore);

	for(int i = 0; i < 6; i++){
		if(buf[i] == '1' || buf[i] == '2' || buf[i] == '3' ||
			buf[i] == '4' || buf[i] == '5' || buf[i] == '6'){
			len++;
		}
	}

	int roll[len];

	for(int i = 0; i < len; i++){
		roll[i] = (int) buf[i] - 48;
	}

	calcDecision(&reroll, &keepScore, roll, len, prevScore);

	printf("Reroll %d dice. Keep a score of %d.\n", reroll, keepScore);
}
