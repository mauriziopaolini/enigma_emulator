#!/bin/bash
#
#testo=`cat testo_loris_giuriatti.txt`
#testo=`echo $testo`

testo="
LKOZW JUCMM XBJRM JTUYB GVCIO IBVBI VCCUT ZUWIV NHAUG
HKRAV BBVKL UKHRJ FEVHN RMHNX ZFKYL QVUBZ GWWXI CCAIL
LZWIM LIVTZ DTHWG ETMCN ZIBQX GMSSR SLDQE ZOTZF TIHLQ
YLDJS QFXAJ XVHKM DFZLS BLTMM PWYUE ZIFTV VOMEI TFLUD
JUNCE BZYBR WWHAO ZEGNU LYFUI YIKQD ODPTA OQEGH SSGKR
JKCPK BRIWP SLKKN FRGJK PIJDF HDZVV NIVVU NZGFD TOVKQ
IOVPE ILHOW AUIVS OSAWQ ICTJH GMZIL CTMOO YJLJF WOVCU
YDUUO MOZBR RJATC IQHIE TAWBM JALZV WIIUP VZWSO EDUSN
FGHJW CEFAB IJEOO RKSLV MDLEC UKQFT OYLQO CGKQ
"

testo=`echo $testo`

#echo "$testo"

#
# https://lorisgiuriatti.it/wp-content/uploads/2018/01/Codice-ENIGMA-Loris-Giuriatti-Urlo-del-Male-01.jpg
#

#
# the first part indicates the settings:
#
# reflector: C
# rotors:    IV II V
# rings:     G E O
# plugs:     JO LA BR ET
#
# the pair: FTF PAT
# is used to find the rotors initial position:
#

#./enigma_emulator --refl C --rotors IV-II-V --rings GEO --plug "JO LA BR ET" --pos FTF -q PAT

newpos=`./enigma_emulator --refl C --rotors IV-II-V --rings GEO --plug "JO LA BR ET" --pos FTF -q --nofp PAT`
echo "New position of rotors: $newpos"
shouldbe="MCG"

if [ "$newpos" != "$shouldbe" ]
then
  echo "WARNING: Initial position should be \"$shouldbe\" instead of \"$newpos\""
fi

#
# so that now the decoding can be obtained with
#

./enigma_emulator --refl C --rotors IV-II-V --rings GEO --plug "JO LA BR ET" --pos $newpos -q $testo

