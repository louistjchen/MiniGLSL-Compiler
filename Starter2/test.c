{

/* Declarations */


int i;
int j;
int k;
bool a;
vec3 myvec;
vec2 myVec = vec2(1.0, 5.0); /* Ok */
vec2 myVec = vec2(1,5); /* Handout says not ok... but still works*/
bvec3 myBVec = bvec3(true,false,true,true);

ivec4 myVec = ivec4(0,1,2,3);
int num = myVec[2];
int test_con = int (1,2,3);

int num = 5.0;
const int num =  5 * (3 + 2);



/* Statements */

tempVec = myVec1 * Vec2;
i = 5;
j = 2 + (2 * 4/2) *k * !i -3-4-5 *(2);

if (i == 5)
{    j = 6+5;

if (j == 6)
    k = 7;
else
    k = 9;
}else
    k = 10;

if (true) {
	if (false) {
		if (false) {
			a = b;
		} else {
			a = 1;
		}
	}
	else {
		/*empty*/
	}
}

while (rsq(1,2,3,t1,t3)) {
	if (t == 2) {

	} else {
		if (false) {
		}
	}
}

}
