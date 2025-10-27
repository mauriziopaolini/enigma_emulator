#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#define MAXTEXTLEN 10000
#define VERSION "1.1"

// static char rotleft[] = "I";
// static char rotmiddle[] = "II";
// static char rotright[] = "III";

static char reflectorl = 'B';

static char rotleftl[4];
static char rotmiddlel[4];
static char rotrightl[4];

static char leftpos = 'A';
static char middlepos = 'A';
static char rightpos = 'A';

static char leftringl = 'A';
static char middleringl = 'A';
static char rightringl = 'A';

// static char plugboards[14];

// static char id[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ@";

/*
 * see: https://en.wikipedia.org/wiki/Enigma_rotor_details
 */

static char rotorI[] = "EKMFLGDQVZNTOWYHXUSPAIBRCJ@";
static char rotorII[] = "AJDKSIRUXBLHWTMCQGZNPYFVOE@";
static char rotorIII[] = "BDFHJLCPRTXVZNYEIWGAKMUSQO@";
static char rotorIV[] = "ESOVPZJAYQUIRHXLNFTGKDCMWB@";
static char rotorV[] = "VZBRGITYUPSDNHLXAWMJQOFECK@";
static char rotorX[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ@";

static char reflectorA[] = "EJMZALYXVBWFCRQUONTSPIKHGD@";
static char reflectorB[] = "YRUHQSLDPXNGOKMIEBFZCWVJAT@";
static char reflectorC[] = "FVPJIAOYEDRZXWGCTKUQSBNMHL@";
static char reflectorX[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ@";

static char plugboard[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ@";

static char rotInotch = 'Q';
static char rotIInotch = 'E';
static char rotIIInotch = 'V';
static char rotIVnotch = 'J';
static char rotVnotch = 'Z';
static char rotXnotch = 'Z';

static int quiet = 0;
static int progress = 0;
static int printfinalpos = 1;

static int reflector;

/*
 * functions prototypes
 */

int rotor2num (char *rotorl);
void rotorsfromoption (char *rotLMR, char *rleft, char *rmiddle, char *rright);
char getnotch (int rotor);
int applyrotor (int number, char *perm, int rr);
int applyrotorinv (int number, char *perm, int rr);
void checkfail (int number, char *msg, int offset);
void insert_defect (char *defect);
void outtext (char *text);

void usage (char *name);

/*
 * main code
 */

int
main (int argc, char *argv[])
{
  int i, j, k, t, len, number, ii, jj, kk;
  int rotleft, rotmiddle, rotright;
  int leftring, middlering, rightring;
  int failtm1 = -1;
  char *cyphertext, *cleartext;
  char posABC[4];
  char rotorsLMR[20];
  char plugboards[41];
  char ringsABC[4];
  char *rot[11], *ref[11];
  char rightnotch, middlenotch, leftnotch;
  char *permleft, *permmiddle, *permright, *permreflector;
  char *chpt, dum;
  int idx1, idx2;

  *posABC = 0;
  *rotorsLMR = 0;
  *ringsABC = 0;
  *plugboards = 0;
  strcpy (rotleftl, "I");
  strcpy (rotmiddlel, "II");
  strcpy (rotrightl, "III");
  reflectorl = 'B';
  rot[0] = rotorI;
  rot[1] = rotorII;
  rot[2] = rotorIII;
  rot[3] = rotorIV;
  rot[4] = rotorV;
  rot[10] = rotorX;
  ref[0] = reflectorA;
  ref[1] = reflectorB;
  ref[2] = reflectorC;
  ref[10] = reflectorX;

  cyphertext = (char *) malloc ( (1 + MAXTEXTLEN)*sizeof(char) );

  for (i = 1; i < argc; i++)
  {
    if (*argv[i] == '-')
    {
      if (strcmp (argv[i], "-h") == 0) {
        usage (argv[0]);
        exit (0);
      } else if (strcmp (argv[i], "--version") == 0) {
        printf ("Version: %s\n", VERSION);
        exit (0);
      } else if (strcmp (argv[i], "-q") == 0) {
        quiet++;
      } else if (strcmp (argv[i], "--nofp") == 0) {
        printfinalpos = 0;
      } else if (strcmp (argv[i], "--progress") == 0) {
        progress++;
      } else if (strcmp (argv[i], "-p") == 0 || strcmp (argv[i], "--pos") == 0) {
        i++;
        assert (strlen (argv[i]) == 3);
        strcpy (posABC, argv[i]);
      } else if (strcmp (argv[i], "-r") == 0 || strcmp (argv[i], "--rotors") == 0) {
        i++;
        assert (strlen (argv[i]) <= 19);
        strcpy (rotorsLMR, argv[i]);
      } else if (strcmp (argv[i], "-R") == 0 || strcmp (argv[i], "--rings") == 0) {
        i++;
        assert (strlen (argv[i]) == 3);
        strcpy (ringsABC, argv[i]);
      } else if (strcmp (argv[i], "--refl") == 0 || strcmp (argv[i], "--reflector") == 0) {
        reflectorl = toupper (*argv[++i]);
      } else if (strcmp (argv[i], "--plug") == 0) {
        i++;
        assert (strlen (argv[i]) <= 40);
        strcpy (plugboards, argv[i]);
      } else if (strcmp (argv[i], "-D") == 0 || strcmp (argv[i], "--defect") == 0) {
        i++;
        insert_defect (argv[i]);
      } else if (strcmp (argv[i], "-F") == 0 || strcmp (argv[i], "--fail") == 0) {
        i++;
        failtm1 = atoi (argv[i]) - 1;


      } else {
        printf ("invalid option: %s\n", argv[i]);
        exit (3);
      }
    } else {
      assert (strlen(cyphertext) + strlen(argv[i]) < MAXTEXTLEN);
      strcat (cyphertext, argv[i]);
    }
  }

  if (strlen (cyphertext) == 0) {usage(argv[0]); exit (1);}

  for (i = 0; i < strlen (cyphertext); i++)
  {
    assert (isalpha(cyphertext[i]));
    cyphertext[i] = toupper (cyphertext[i]);
  }

  if (*rotorsLMR) {
    rotorsfromoption (rotorsLMR, rotleftl, rotmiddlel, rotrightl);

    if (!quiet) printf ("Option -r supersedes default values, left-middle-right rotors: %s-%s-%s\n",
      rotleftl, rotmiddlel, rotrightl);
  }

  rotleft = rotor2num (rotleftl);
  rotmiddle = rotor2num (rotmiddlel);
  rotright = rotor2num (rotrightl);

  reflector = reflectorl - 'A';
  if (reflectorl == 'X') reflector = 10;
  if ((reflector < 0 || reflector > 2) && reflectorl != 'X') {
    printf ("Invalid Reflector selected: %c -> %d\n", reflectorl, reflector);
    exit (2);
  }

  if (*posABC != 0)
  {
    leftpos = toupper (posABC[0]);
    middlepos = toupper (posABC[1]);
    rightpos = toupper (posABC[2]);
    if (!quiet) printf ("Option -p supersedes default values, positions: %c %c %c\n", leftpos, middlepos, rightpos);
  }

  if (*ringsABC != 0) {
    assert (strlen (ringsABC) == 3);
    leftringl = toupper (ringsABC[0]);
    middleringl = toupper (ringsABC[1]);
    rightringl = toupper (ringsABC[2]);
    if (!quiet) printf ("Option -R supersedes default values, rings setting: %c %c %c\n", leftringl, middleringl, rightringl);
  }

  if (*plugboards != 0) {
    chpt = plugboards;
    while (chpt) {
      while (isspace(*chpt)) chpt++;
      if (*chpt == 0) break;
      assert (isalpha(*chpt));
      idx1 = toupper(*chpt++) - 'A';
      assert (isalpha(*chpt));
      idx2 = toupper(*chpt++) - 'A';
      assert (idx1 != idx2);
      if (plugboard[idx1] == '@') plugboard[idx2] = '@';
      if (plugboard[idx2] == '@') plugboard[idx1] = '@';
      dum = plugboard[idx1];
      plugboard[idx1] = plugboard[idx2];
      plugboard[idx2] = dum;
    }
    if (!quiet) printf ("Option --plug supersedes default values, resulting permutation: %s\n", plugboard);
  }

  leftring = leftringl - 'A';
  middlering = middleringl - 'A';
  rightring = rightringl - 'A';

  permleft = rot[rotleft];
  permmiddle = rot[rotmiddle];
  permright = rot[rotright];
  permreflector = ref[reflector];

  rightnotch = getnotch (rotright) - 'A';
  middlenotch = getnotch (rotmiddle) - 'A';
  leftnotch = getnotch (rotleft) - 'A';
  assert (leftnotch);  // this is just to avoid a compilation warning

  if (!quiet) printf ("\ninput cypher text: %s\n", cyphertext);

  /* ==================== */

  i = leftpos - 'A';
  j = middlepos - 'A';
  k = rightpos - 'A';

  len = strlen (cyphertext);

  cleartext = (char *) malloc ((1 + len)*sizeof(char));

  for (t = 0; t < len; t++)
  {
    if (t == failtm1) printf ("\n=== Failing decode for this input letter: %c at position %d ===\n", cyphertext[t], failtm1);
    if (j == middlenotch) {
      if (! quiet) fprintf (stderr, "Double stepping [ijk] = [%d %d %d]\n", i, j, k);
      i = (i + 1) % 26;
    }
    if (k == rightnotch || j == middlenotch) {
      j = (j + 1) % 26;
    }

    k = (k + 1) % 26;

    ii = (i + 26 - leftring) % 26;
    jj = (j + 26 - middlering) % 26;
    kk = (k + 26 - rightring) % 26;

    number = cyphertext[t] - 'A';
    if (t == failtm1) checkfail (number, "plugboard", 0);
    if (number >= 0) number = plugboard[number] - 'A';
    if (t == failtm1) checkfail (number, "right rotor", kk);
    if (number >= 0) number = applyrotor (number, permright, kk);
    if (t == failtm1) checkfail (number, "middle rotor", jj);
    if (number >= 0) number = applyrotor (number, permmiddle, jj);
    if (t == failtm1) checkfail (number, "left rotor", ii);
    if (number >= 0) number = applyrotor (number, permleft, ii);
    if (t == failtm1) checkfail (number, "reflector", 0);
    if (number >= 0) number = permreflector[number] - 'A';
    if (t == failtm1) checkfail (number, "reflector", 0);
    if (number >= 0) number = applyrotorinv (number, permleft, ii);
    if (t == failtm1) checkfail (number, "left rotor", ii);
    if (number >= 0) number = applyrotorinv (number, permmiddle, jj);
    if (t == failtm1) checkfail (number, "middle rotor", jj);
    if (number >= 0) number = applyrotorinv (number, permright, kk);
    if (t == failtm1) checkfail (number, "right rotor", kk);
    if (number >= 0) number = plugboard[number] - 'A';
    if (t == failtm1) checkfail (number, "plugboard", 0);

    cleartext[t] = number + 'A';
    if (number < 0) cleartext[t] = '?';
    if (t == failtm1) cleartext[t] = '?';
  }

  if (printfinalpos || !quiet) printf ("\n");
  if (printfinalpos) {
    printf ("Final rotor positions: %c %c %c\n", i + 'A', j + 'A', k + 'A');
  }
  if (!quiet) printf ("Cleartext:\n");
  if (!quiet) printf ("====================\n");
  outtext (cleartext);
  if (!quiet) printf ("====================\n");
}


int
rotor2num (char *rotorl)
{
  if (strcmp (rotorl, "I") == 0) return 0;
  if (strcmp (rotorl, "II") == 0) return 1;
  if (strcmp (rotorl, "III") == 0) return 2;
  if (strcmp (rotorl, "IV") == 0) return 3;
  if (strcmp (rotorl, "V") == 0) return 4;
  if (strcmp (rotorl, "X") == 0) return 10;

  fprintf (stderr, "Invalid rotor choice: %s\n", rotorl);
  exit (0);
  return (-1);
}

void
rotorsfromoption (char *rotLMR, char *rleft, char *rmiddle, char *rright)
{
  char *strpt, *rleftpt, *rmiddlept, *rrightpt;

  strpt = rotLMR;
  for (rleftpt = rleft; *strpt != 0 && *strpt != '-'; strpt++)
  {
    *rleftpt++ = *strpt;
  }
  *rleftpt = 0;
  assert (*strpt++ == '-');

  for (rmiddlept = rmiddle; *strpt != 0 && *strpt != '-'; strpt++)
  {
    *rmiddlept++ = *strpt;
  }
  *rmiddlept = 0;
  assert (*strpt++ == '-');

  for (rrightpt = rright; *strpt != 0 && *strpt != '-'; strpt++)
  {
    *rrightpt++ = *strpt;
  }
  *rrightpt = 0;
  assert (*strpt == 0);

  return ;
}

char
getnotch (int rotor)
{
  if (rotor == 0) return rotInotch;
  if (rotor == 1) return rotIInotch;
  if (rotor == 2) return rotIIInotch;
  if (rotor == 3) return rotIVnotch;
  if (rotor == 4) return rotVnotch;
  if (rotor == 10) return rotXnotch;

  fprintf (stderr, "Invalid rotor number: %d\n", rotor);
  exit (7);
  return (0);
}

int
applyrotor (int num, char *perm, int rr)
{
  if (num < 0) return (num);
  num = (num + rr) % 26;
  num = perm[num] - 'A';
  if (num < 0) return (num);
  num = (num - rr + 26) % 26;
  return (num);
}

int
applyrotorinv (int num, char *perm, int rr)
{
  int i, num2;
  int atfound = 0;

  if (num < 0) return (num);
  num = (num + rr) % 26;
  num2 = -1;
  for (i = 0; i < 26; i++)
  {
    if (perm[i] == '@') atfound++;
    if (perm[i] - 'A' == num)
    {
      assert (num2 < 0);
      num2 = i;
    }
  }
  assert (num2 >= 0 || atfound);
  if (num2 < 0) return (-1);
  num = (num2 - rr + 26) % 26;
  return (num);
}

void
checkfail (int number, char *msg, int offset)
{
  int numoff;

  if (number < 0) {
    printf ("Real failure: cannot check virtual failure\n");
    return;
  }
  numoff = (number + offset + 26) % 26;
  printf ("Circuit failure possibly at %s for letter %c\n", msg, numoff + 'A');
}

void
insert_defect (char *defect)
{
  char *letters, *chpt;
  char *perm;
  int number, isreflector = 0;

  for (chpt = defect; *chpt && (*chpt != ':'); chpt++);

  assert (*chpt == ':');
  *chpt++ = 0;

  letters = chpt;

  if (strcmp (defect, "I") == 0) perm = rotorI;
  else if (strcmp (defect, "II") == 0) perm = rotorII;
  else if (strcmp (defect, "III") == 0) perm = rotorIII;
  else if (strcmp (defect, "IV") == 0) perm = rotorIV;
  else if (strcmp (defect, "V") == 0) perm = rotorV;
  else if (strcmp (defect, "A") == 0) {perm = reflectorA; isreflector++;}
  else if (strcmp (defect, "B") == 0) {perm = reflectorB; isreflector++;}
  else if (strcmp (defect, "C") == 0) {perm = reflectorC; isreflector++;}
  else if (strcmp (defect, "P") == 0) perm = plugboard;

  else {
    fprintf (stderr, "Invalid defective object: %s\n", defect);
    exit (5);
  }

  for (chpt = letters; *chpt; chpt++) {
    number = toupper (*chpt) - 'A';
    assert (number >= 0 && number < 26);
    perm[perm[number] - 'A'] = '@';
    perm[number] = '@';
  }
  //printf ("what:%s - letters: %s\n", defect, letters);
  //printf ("defective perm: %s\n", perm);
}

void
outtext (char *text)
{
  char *chpt;
  int t = 0;

  for (t = 0, chpt = text; *chpt; chpt++, t++)
  {
    if (t > 0 && (t % 5) == 0) printf (" ");
    if (t > 0 && (t % 50) == 0) printf ("\n");
    printf ("%c", *chpt);
  }
  printf ("\n");
}

void
usage (char *name)
{
  printf ("usage: %s [-h][-q][--progress][-p|--pos <initialpos>][-r|--rotors <rotors>]\n", name);
  printf ("  [-R|--rings <rings>][--refl A|B|C][--plug \"XY ZT ...\"][--nofp][-F|--fail <t>]\n");
  printf ("  [-D|--defect <defect>] <cyphertext>\n\n");
  printf ("-p|--pos: Rotors displayed letters\n");
  printf ("-r|--rotors: Rotors choice and ordering (left-to-right)\n");
  printf ("-R|--rings: Rings settings\n");
  printf ("--nofp: Do not print final rotors displayed letters\n");
  printf ("-F|--fail: Simulate a circuit error for character at position <t> in the cyphertext\n\n");
  printf ("<rotors> is a sequence from I II III IV V separated by a '-', as in 'I-II-III'\n");
  printf ("<initialpos> and <rings> is a sequence of three uppercase letters\n");
  printf ("<defect> is a string like <what>:<letters> where <what> can be one of I, II, III, IV, V, A, B, C, P\n");
  printf ("   indicating one rotor or a reflector or a plug cable/machine wiring\n");
  printf ("   and <letters> is one or more letters indicating what wire is at fail\n");
}

