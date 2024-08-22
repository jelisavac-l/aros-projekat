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


