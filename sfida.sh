#!/bin/bash
#
# sfida lanciata in occasione della "settimana della scienza 2025" 21-27/10/2025

testo="DNEZQ CIBCE GIUET VELKE MSLTD WMCTA UOWFX BCTGH IUNIQ GIPYH GMGLU MWZFN FCZJY RUHTJ WXETC I"
soluzione="LASOL UZION EDIQU ESTAS FIDAE ILNOM EDELL ANIMA LECHE TROVI INFON DOAQU ESTOT ESTON ARVAL O"

./enigma_emulator --rotors I-V-II --rings BAR --pos TAC $testo

echo ""
echo "============================================"
echo "confronta la soluzione ottenuta con il testo:"
echo "$soluzione"
echo "============================================"
