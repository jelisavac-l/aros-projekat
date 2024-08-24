# AROS Projekat: Interprocesna komunikacija sa semaforima za razmenu fajlova.

> Nastavak projekta će biti hostovan na ovom github nalogu koji su mi fina gospoda iz Microsofta vratila nakon incidenta sa infostealerom. Hvala im! 💛

## Projekat 💡

Minimalna file client/server arhitektura za razmenu fajlova među procesima koja demonstrira interprocesnu komunikaciju na UNIX operativnim sistemima upotrebom MessageQueue mehanizma i semafora.

### Ideja

Postoje dva programa, server (`./server`) i klijent (`./client`). Slično troslojnoj arhitekturi koja je princip funkcionisanja web-sajtova, klijent šalje zahtev za fajl, server obrađuje zahtev i šalje odgovor.

Request-response ciklus se odvija na sledeći način:

1. Pokreće se server koji otvara svoj MessageQueue sa ključem unapred poznatim svakom klijentu jer se generiše `ftok` funkcijom za zadati path poznat svim klijentskim procesima.
2. Pokreće se klijent koji otvara svoj MessageQueue i od korisnika zahteva naziv fajla. Zatim, klijent šalje podatke u formatu `klijent_mq:naziv_fajla` sistemskim pozivom `msgsnd` na MessageQueue koji pripada serveru.
3. Server prima informaciju na koji MessageQueue treba da šalje odgovor odnosno fajl.
4. Server "razgrađuje" traženi fajl u niz bajtova i šalje veličinu fajla za prenos (radi parsiranja), a potom po principu bajt po bajt šalje fajl na klijentov MessageQueue. Ovaj proces služi da bi mogli da se šalju kako tekstualni tako i binarni fajlovi.
5. Po završetku slanja fajla, server šalje poruku `END` kako bi označio da je poslat kompletan fajl.
6. Klijent po primljenoj poruci `END` zatvara svoj MessageQueue, parsira bajtove i "sastavlja" ih nazad u fajl.
## Build & Kompilacija ⚙️
### Preduslovi
Ključne funkcionalnosti ovog projekta zavise od POSIX biblioteka za programski jezik C te se podrazumeva da je jedino moguće kompajlirati ga na nekom od UNIX-ovih operativnih sistema (testirano na distribucijama KUbuntu iz Debian porodice i EndeavourOS iz Arch porodice). Drugim rečima, nije moguće kompajlirati projekat na Windows-u. Takođe, neophodni su `GCC` (GNU C Compiler) i `Make` koji su gotovo svim distribucijama instalirani sa sistemom.

### Kompilacija
> Note to self: optimizovati ovaj proces pre finalne verzije i odbrane
1. Kompajlirati spoljne biblioteke `make libs`
2. Kompajlirati pomoćne biblioteke `make utils`
3. Kompajlirati UI biblioteke `make uiplusplus`
4. Kompajlirati klijent `make client`
5. Kompajlirati server `make server`

## Upotreba 🍀
### Pokretanje
1. Pokrenuti server u modu za slušanje `./server -l`.
2. Pokrenuti klijent u modu za zahteve `./client -r` i pratiti uputstva.

### Gašenje
1. U terminalu u kome se server izvršava, proslediti interrupt signal pritiskom `Ctrl + C`
2. Proveriti da li su svi MessageQueue-ovi ovog projekta ugašeni naredbom `ipcs` i u slučaju da je došlo do greške i neki nije ispravno zatvoren, ručno ga zatvoriti naredbom `ipcrm`.

## Poznate greške ⚠️
> Koje trebaju biti ispravljene (ili vešto sakrivene) pre odbrane
* Zahtevanje nepostojećeg fajla rezultira u segfault-u. Neophodno je ručno zatvaranje MQ-ova.
* Pokretanje klijenta pre servera pravi opšti haos oko kreiranja serverskog MQ-a.
* "Nasilno" gašenje klijenta pre završetka komunikacije može da zakoči server.