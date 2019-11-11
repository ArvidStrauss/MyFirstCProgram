#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h>
#include <ctype.h> 
#include <string.h> 
#include <stdlib.h> 
#include <memory.h>
#include <time.h>

#define N_START 3
#define N_DELTA 5 
#define MAX_YEAR 2022

//scanf("%c", &c); 
//\x81=ü, \x84=ä,

char eingabe_ausw()
{
	char zeile[20]; //alternativ: fgets(zeile, 20, stdin);
	scanf("%s", zeile);

	while (zeile[0] == ' ')
		strcpy(zeile, &zeile[1]);
	zeile[0] = tolower(zeile[0]);

	return zeile[0];
}

typedef struct
{
	int tag, monat, jahr;
}datum;

typedef struct
{
	int kdnr, bnr, kat, hausnr;
	datum anreise, abreise;
}buchung_t;

typedef struct
{
	buchung_t *array_ptr;
	int n_elements;
	int n_allocated;
}dynarray_handle;

void anzeigen(dynarray_handle dh)
{
	int i;
	printf("\n\t++Buchungsuebersicht++\n Buchungen: %d gesamt, %d allocated \n\n", dh.n_elements, dh.n_allocated);

	for (i = 0; i<dh.n_elements; i++)
		printf("Buchungsnr.: %07d\nKundennummer: %07d\nKategorie: %d\nHausnummer: %d\nZeitraum vom: %02d.%02d.%4d bis: %02d.%02d.%4d\n\n", dh.array_ptr[i].bnr, dh.array_ptr[i].kdnr, dh.array_ptr[i].hausnr, dh.array_ptr[i].kat, dh.array_ptr[i].anreise.tag, dh.array_ptr[i].anreise.monat, dh.array_ptr[i].anreise.jahr, dh.array_ptr[i].abreise.tag, dh.array_ptr[i].abreise.monat, dh.array_ptr[i].abreise.jahr);
}

int schaltjahr(int jahr)
{
	if (jahr % 4 == 0 && (jahr % 100 != 0 || jahr % 400 == 0))
		return 1;
	else
		return 0;
}

int tagesnr(datum *d)
{
	int schalt = schaltjahr(d->jahr);
	int tageszahl;
	switch (d->monat)
	{
	case 1: tageszahl = d->tag + ((d->jahr) * 1000); break;
	case 2: tageszahl = 31 + schalt + d->tag + ((d->jahr) * 1000); break;
	case 3: tageszahl = 31 + schalt + 28 + d->tag + ((d->jahr) * 1000); break;
	case 4: tageszahl = 31 + schalt + 28 + 31 + d->tag + ((d->jahr) * 1000); break;
	case 5: tageszahl = 31 + schalt + 28 + 31 + 30 + d->tag + ((d->jahr) * 1000); break;
	case 6: tageszahl = 31 + schalt + 28 + 31 + 30 + 31 + d->tag + ((d->jahr) * 1000); break;
	case 7: tageszahl = 31 + schalt + 28 + 31 + 30 + 31 + 30 + d->tag + ((d->jahr) * 1000); break;
	case 8: tageszahl = 31 + schalt + 28 + 31 + 30 + 31 + 30 + 31 + d->tag + ((d->jahr) * 1000); break;
	case 9: tageszahl = 31 + schalt + 28 + 31 + 30 + 31 + 30 + 31 + 31 + d->tag + ((d->jahr) * 1000); break;
	case 10: tageszahl = 31 + schalt + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + d->tag + ((d->jahr) * 1000); break;
	case 11: tageszahl = 31 + schalt + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + d->tag + ((d->jahr) * 1000); break;
	case 12: tageszahl = 365 - 31 + schalt + d->tag + ((d->jahr) * 1000); break;
	default: tageszahl = 0; break;
	}
	return tageszahl;
}

int zeitraum_pruefung(datum *anreise, datum *abreise)
{
	if (tagesnr(anreise) > tagesnr(abreise))
		return 1;
	else
		return 0;
}

int reisedauer(datum *anreise, datum *abreise)
{
	int reisedauer;
	if(anreise->jahr < abreise->jahr)
		reisedauer= tagesnr(abreise) - tagesnr(anreise) + 365 + schaltjahr(anreise->jahr) - ((abreise->jahr-anreise->jahr) * 1000);
	else
		reisedauer = tagesnr(abreise) - tagesnr(anreise);
	return reisedauer;
}

int eingabe_datum(datum *a, struct tm *ts)
{
	scanf("%d.%d.%d", &a->tag, &a->monat, &a->jahr);

	if (a->jahr < ts->tm_year + 1900)
		return 1;
	if (a->monat < ts->tm_mon +1 && a->jahr == ts->tm_year + 1900)
		return 1;
	if (a->tag < ts->tm_mday && a->jahr == ts->tm_year + 1900 && a->monat == ts->tm_mon+1)
		return 1;

	if (a->monat < 1 || a->monat > 12)
		return 1;

	if (a->jahr > MAX_YEAR)
		return 2;

	if (a->monat == 2)
	{
		if (schaltjahr(a->jahr) == 0)
		{
			if (a->tag > 28 || a->tag <1)
				return 1;
		}
		else
		{
			if (a->tag > 29 || a->tag <1)
				return 1;
		}
	}

	else
	{
		if (a->monat == 4 || a->monat == 6 || a->monat == 9 || a->monat == 11)
		{
			if (a->tag > 30 || a->tag < 1)
				return 1;
		}
		else
		{
			if (a->tag > 31 || a->tag < 1)
				return 1;
		}
	}
	return 0;
}

int hausnr_pruefen(int hausnr)
{
	if (hausnr >= 1 && hausnr <= 10)
		return 0;
	else
		return 1;

}

void hausbelegung_pruefen1(dynarray_handle *dh, datum *anreise, datum *abreise, int kat)
{
	int i, j = 0, tagesnr_anreise_kunde = 0, tagesnr_abreise_kunde = 0;
	int belegte_haueser[10] = { 0 };

	if (dh->n_elements > 0)
	{
		for (i = 0; i <= dh->n_elements; i++)
		{
			if (dh->array_ptr[i].kat == kat)
			{
				if (!(tagesnr(abreise) < tagesnr(&(dh->array_ptr[i].anreise)) || tagesnr(&(dh->array_ptr[i].abreise)) < tagesnr(anreise)))
				{
					for (j = 0; j < 10; j++)
					{
						if (dh->array_ptr[i].hausnr == j + 1)
							belegte_haueser[j] = 1;
					}
				}
			}
		}
	}

	printf("\nFreie Haeuser in der gewuenschten Kategorie:\n");
	for (i = 0; i < 10; i++)
	{
		if (belegte_haueser[i] == 0)
			printf("%d ", i + 1);
	}
}

int hausbelegung_pruefen2(dynarray_handle *dh, datum *anreise, datum *abreise, int kat, int hausnr)
{
	int i;

	if (dh->n_elements>0)
	{
		for (i = 0; i <= dh->n_elements; i++)
		{
			if (dh->array_ptr[i].kat == kat && dh->array_ptr[i].hausnr == hausnr)
			{
				if (!(tagesnr(abreise) < tagesnr(&(dh->array_ptr[i].anreise)) || tagesnr(&(dh->array_ptr[i].abreise)) < tagesnr(anreise)))
					return 1;
			}
		}
	}
	return 0;
}

void hausbelegung_pruefen3(dynarray_handle *dh, datum *anreise, datum *abreise)
{
	int i, j, k;
	double haus_a = 0, haus_b = 0, haus_c = 0;

	if (dh->n_elements > 0)
	{
		for (i = 0; i <= dh->n_elements; i++)
		{
			for (k = tagesnr(anreise); k <= tagesnr(anreise) + reisedauer(anreise, abreise); k++)
			{
				for (j = tagesnr(&(dh->array_ptr[i].anreise)); j <= tagesnr(&(dh->array_ptr[i].anreise)) + reisedauer(anreise, abreise); j++)
				{
					if (k == j)
					{
						if (dh->array_ptr[i].kat == 1)
							haus_a++;
						if (dh->array_ptr[i].kat == 2)
							haus_b++;
						if (dh->array_ptr[i].kat == 3)
							haus_c++;
						printf("a: %lf b: %lf c: %lf\n", haus_a, haus_b, haus_c);
					}

				}
			
			}
		}
	}

	printf("Auslastung von A: %.2lf Prozent", haus_a / ((1 + reisedauer(anreise, abreise)) * 10) * 100);
	printf("Auslastung von B: %.2lf Prozent", haus_b / ((1 + reisedauer(anreise, abreise)) * 10) * 100);
	printf("Auslastung von C: %.2lf Prozent", haus_c / ((1 + reisedauer(anreise, abreise)) * 10) * 100);
}

int einfuegen(dynarray_handle *dh, int kdnr, int bnr, int kat, int hausnr, datum *anreise, datum *abreise)
{
	if (dh->n_allocated == 0)
	{
		dh->array_ptr = (buchung_t*)malloc(sizeof(buchung_t)* N_START);
		if (dh->array_ptr == NULL)
			return 1;
		dh->n_allocated = N_START;
	}

	if (dh->n_elements + 1> dh->n_allocated)
	{
		dh->array_ptr = (buchung_t*)realloc(dh->array_ptr, sizeof(buchung_t) * (dh->n_allocated + N_DELTA));
		if (dh->array_ptr == NULL)
			return 1;
		dh->n_allocated = dh->n_allocated + N_DELTA;
	}

	dh->array_ptr[dh->n_elements].hausnr = hausnr;
	dh->array_ptr[dh->n_elements].kat = kat;
	dh->array_ptr[dh->n_elements].anreise.tag = anreise->tag;
	dh->array_ptr[dh->n_elements].anreise.monat = anreise->monat;
	dh->array_ptr[dh->n_elements].anreise.jahr = anreise->jahr;
	dh->array_ptr[dh->n_elements].abreise.tag = abreise->tag;
	dh->array_ptr[dh->n_elements].abreise.monat = abreise->monat;
	dh->array_ptr[dh->n_elements].abreise.jahr = abreise->jahr;
	dh->array_ptr[dh->n_elements].bnr = bnr;
	dh->array_ptr[dh->n_elements].kdnr = kdnr;
	dh->n_elements = dh->n_elements + 1;

	return 0;
}

int loeschen(dynarray_handle *dh, int kdnr, int bnr)
{
	int i, tmp = -1;

	if (dh->n_elements > 0)
	{
		for (i = 0; i <= dh->n_elements; i++)
			if (dh->array_ptr[i].bnr == bnr && dh->array_ptr[i].kdnr == kdnr)
			{
				tmp = i;
				break;
			}
	}

	if (tmp >= 0)
	{
		for (i = tmp + 1; i <= dh->n_elements; i++)
			dh->array_ptr[i - 1] = dh->array_ptr[i];
		dh->n_elements = dh->n_elements - 1;

		if (dh->n_elements < dh->n_allocated - N_DELTA)
		{
			dh->array_ptr = (buchung_t*)realloc(dh->array_ptr, sizeof(buchung_t) * (dh->n_allocated - N_DELTA));
			dh->n_allocated = dh->n_allocated - N_DELTA;
		}

		return 0;

	}
	else
		return 1;
}

void anfrage_option1(dynarray_handle *dh, datum *anreise, datum *abreise, struct tm *ts)
{
	int kat, check1, ende = 0, check2;
	char ausw;

	printf("Anfrage-Option 1:\n");

	do
	{
		printf("\nHauskategorie w\x84hlen: [A], [B] oder [C]\n");
		ausw = eingabe_ausw();

		switch (ausw)
		{
		case 'a':
			kat = 1;
			ende = 1;
			break;
		case 'b':
			kat = 2;
			ende = 1;
			break;
		case 'c':
			kat = 3;
			ende = 1;
			break;
		default:
			printf("Ung\x81ltige Eingabe\n\n\a");
		}
	} while (ende == 0);

	do
	{
		printf("Bitte zu ueberpruefendes Anreisedatum eingeben:\n");
		check1 = eingabe_datum(anreise, ts);
		if (check1 == 1)
			printf("Ungueltiges Datum!\a\n\n");
	} while (check1 == 1);

	do
	{
		printf("Bitte zu ueberpruefendes Abreisedatum eingeben:\n");
		check2 = eingabe_datum(abreise, ts);
		if (check2 == 1)
			printf("Ungueltiges Datum!\a\n\n");
	} while (check2 == 1);

	hausbelegung_pruefen1(dh, anreise, abreise, kat);
}

void anfrage_option2(dynarray_handle *dh, datum *anreise, datum *abreise, struct tm *ts)
{
	int kat, hausnr, check1, check2, check3, ende = 0, rc;
	char ausw;

	do
	{
		printf("\nHauskategorie w\x84hlen: [A], [B] oder [C]\n");
		ausw = eingabe_ausw();

		switch (ausw)
		{
		case 'a':
			kat = 1;
			ende = 1;
			break;
		case 'b':
			kat = 2;
			ende = 1;
			break;
		case 'c':
			kat = 3;
			ende = 1;
			break;
		default:
			printf("Ung\x81ltige Eingabe\n\n\a");
		}
	} while (ende == 0);

	ende = 0;

	do
	{
		printf("\nHausnummer waehlen [1] bis [10]\n");
		scanf("%d", &hausnr);
		check1 = hausnr_pruefen(hausnr);
		if (check1 == 1)
			printf("Ungueltige Hausnummer!\a\n");
	} while (check1 == 1);

	do
	{
		printf("\nBitte gewuenschtes Anreisedatum eingeben:\n");
		check2 = eingabe_datum(anreise, ts);
		if (check2 == 1)
			printf("\nUngueltiges Datum!\a\n\n");
	} while (check2 == 1);

	do
	{
		printf("\nBitte gewuenschtes Abreisedatum eingeben:\n");
		check3 = eingabe_datum(abreise, ts);
		if (check3 == 1)
			printf("\nUngueltiges Datum!\a\n\n");
	} while (check3 == 1);

	rc = hausbelegung_pruefen2(dh, anreise, abreise, kat, hausnr);
	if (rc == 1)
		printf("\nHaus ist belegt.\a\n");
	else
		printf("\nHaus ist frei.\n");
}

void anfrage_option3(dynarray_handle *dh, datum *anreise, datum *abreise, struct tm *ts)
{
	int check1, check2;
	do
	{
		printf("\nBitte 1. Datum eingeben:\n");
		check1 = eingabe_datum(anreise, ts);
		if (check1 == 1)
			printf("\nUngueltiges Datum!\a\n\n");
	} while (check1 == 1);

	do
	{
		printf("\nBitte 2. Datum eingeben:\n");
		check2 = eingabe_datum(abreise, ts);
		if (check2 == 1)
			printf("\nUngueltiges Datum!\a\n\n");
	} while (check2 == 1);

	hausbelegung_pruefen3(dh, anreise, abreise);
}

void anfrage(dynarray_handle *dh, datum *anreise, datum *abreise, struct tm *ts)
{
	int ende = 0;
	char ausw;

	do
	{
		printf("\nAnfrage-Optionen ... \n[1]Anfrage, ob Haeuser in einer best. Kategorie frei sind. \n[2]Anfrage, ob ein gewuenschtes Haus fuer einen Zeitraum frei ist.\n[3]Belegungsanteil in Prozent fuer Zeitraum. \n[Z]urueck\n");
		ausw = eingabe_ausw();

		switch (ausw)
		{
		case '1':
			anfrage_option1(dh, anreise, abreise, ts);
			break;
		case '2':
			anfrage_option2(dh, anreise, abreise, ts);
			break;
		case '3':
			anfrage_option3(dh, anreise, abreise, ts);
			break;
		case 'z':
			ende = 1;
			break;
		default:
			printf("\nUng\x81ltige Eingabe\n\n\a");
		}
	} while (ende == 0);
}

void buchung(dynarray_handle *dh, datum *anreise, datum *abreise,struct tm *ts )
{
	int i, kdnr, bnr, kat, hausnr, check1, check2, check3, check4, check5, check6, ende1 = 0, ende2 = 0;
	char ausw;

	do
	{
		printf("Sind Sie bereits Kunde bei uns? [J]a oder [N]ein\n");
		ausw = eingabe_ausw();

		switch (ausw)
		{
		case 'j':
			printf("Bitte Kundennummer eingeben:");
			scanf("%d", &kdnr);
			for (i = 0; i < dh->n_elements; i++)
			{
				if (dh->array_ptr[i].kdnr == kdnr)
				{
					printf("Kunde gefunden!");
					ende1 = 1;
					break;
				}
			}
			if (ende1 == 0)
				printf("Kundennummer nicht bekannt");
			break;
		case 'n':
			kdnr = dh->n_elements + 1;
			ende1 = 1;
			break;
		default:
			printf("Ung\x81ltige Eingabe\n\n\a");
		}
	} while (ende1 == 0);

	do
	{
		printf("\nHauskategorie w\x84hlen: [A], [B] oder [C]\n");
		ausw = eingabe_ausw();

		switch (ausw)
		{
		case 'a':
			kat = 1;
			ende2 = 1;
			break;
		case 'b':
			kat = 2;
			ende2 = 1;
			break;
		case 'c':
			kat = 3;
			ende2 = 1;
			break;
		default:
			printf("Ung\x81ltige Eingabe\n\n\a");
		}
	} while (ende2 == 0);

	do
	{
		printf("\nHausnummer waehlen [1] bis [10]\n");
		scanf("%d", &hausnr);
		check1 = hausnr_pruefen(hausnr);
		if (check1 == 1)
			printf("Ungueltige Hausnummer!\a\n");
	} while (check1 == 1);

	do
	{
		do
		{
			do
			{
				printf("\nBitte Anreisedatum eingeben:");
				check2 = eingabe_datum(anreise, ts);
				if (check2 == 1)
					printf("\nUngueltiges Datum\n\n\a");
				if (check2 == 2)
					printf("\nDatum zu weit in der Zukunft\n\n\a");
			} while (check2 == 1 || check2 == 2);

			do
			{
				printf("\nBitte Abreisedatum eingeben:");
				check3 = eingabe_datum(abreise, ts);
				if (check3 == 1)
					printf("\nUngueltiges Datum\n\n\a");
				if (check3 == 2)
					printf("\nDatum zu weit in der Zukunft\n\n\a");
			} while (check3 == 1 || check3 == 2);

			check4 = zeitraum_pruefung(anreise, abreise);
			if (check4 == 1)
				printf("\nUngueltiger Zeitraum\n\n\a");
			//else
			//printf("Zeitraum: %02d.%02d.%4d - %02d.%02d.%4d\n", anreise->tag, anreise->monat, anreise->jahr, abreise->tag, abreise->monat, abreise->jahr);
			check6 = reisedauer(anreise, abreise);
			if (check6 < 3)
				printf("Dauer zu kurz. Bitte Reisedauer von mind. 3 Tagen angeben!\a\n");
			printf("Reisedauer: %d Tage\n", check6);
			
		} while (check4 == 1 || check6 < 3);

		check5 = hausbelegung_pruefen2(dh, anreise, abreise, kat, hausnr);
		if (check5 == 1)
			printf("Haus ist schon ausgebucht in dem Zeitraum\n\a");

	} while (check5 == 1);

	bnr = dh->n_elements + 1;

	einfuegen(dh, kdnr, bnr, hausnr, kat, anreise, abreise);

	printf("\nIhre Buchung war erfolgreich.\nBuchungsnr.: %07d\nKundennummer: %07d\nKategorie: %d\nHausnummer: %d\nZeitraum vom: %02d.%02d.%04d bis: %02d.%02d.%04d\n\n", dh->array_ptr[dh->n_elements - 1].bnr, dh->array_ptr[dh->n_elements - 1].kdnr, dh->array_ptr[dh->n_elements - 1].hausnr, dh->array_ptr[dh->n_elements - 1].kat, dh->array_ptr[dh->n_elements - 1].anreise.tag, dh->array_ptr[dh->n_elements - 1].anreise.monat, dh->array_ptr[dh->n_elements - 1].anreise.jahr, dh->array_ptr[dh->n_elements - 1].abreise.tag, dh->array_ptr[dh->n_elements - 1].abreise.monat, dh->array_ptr[dh->n_elements - 1].abreise.jahr);
}

void storno(dynarray_handle *dh)
{
	int rc;
	printf("\nStornierung einer Buchung\n");
	int kdnr, bnr;
	printf("\nBitte Kundennummer eingeben:\n");
	scanf("%d", &kdnr);
	printf("\nBitte Buchungsnummer eingeben:\n");
	scanf("%d", &bnr);

	rc = loeschen(dh, kdnr, bnr);
	if (rc == 0)
		printf("\nStornierung erfolgreich!\n");
	else
		printf("\nBuchung nicht gefunden. Keine Stornierung moeglich!\a\n");
}

int main()
{
	int i, ende = 0;
	char ausw;

	time_t t;
	struct tm * ts;
	t = time(NULL);
	ts = localtime(&t);

	datum anreise = { 0 };
	datum abreise = { 0 };

	dynarray_handle dh = { NULL, 0, 0 };

	//const char ausgabedatei[] = "T:\\Temp\\daten_ausgabe.txt";
	//const char hilfsarray[] = "T:\\Temp\\hilfsarray.txt";
	const char ausgabedatei[] = "C:\\Studium Wirtschaftsinformatik 1718\\daten_ausgabe.txt";
	const char hilfsarray[] = "C:\\Studium Wirtschaftsinformatik 1718\\hilfsarray.txt";

	FILE *ein, *ein2, *aus, *aus2;

	ein2 = fopen(hilfsarray, "rt");
	if (ein2 == NULL)
		printf("Datei %s noch nicht vorhanden. Wird bei Beendigung des Programms angelegt, wenn moeglich.\n", hilfsarray);
	else
	{
		fscanf(ein2, "%d %d", &dh.n_allocated, &dh.n_elements);
		//printf("allocated %d elements %d", dh.n_allocated, dh.n_elements);
		fclose(ein2);
	}

	ein = fopen(ausgabedatei, "rt");
	if (ein == NULL)
		printf("Datei %s noch nicht vorhanden. Wird bei Beendigung des Programms angelegt, wenn moeglich\n", ausgabedatei);
	else
	{
		dh.array_ptr = (buchung_t*)malloc(dh.n_allocated * sizeof(buchung_t));
		if (dh.array_ptr == NULL)
		{
			printf("Fehler bei Speicherallokation\n");
			return 1;
		}

		for (i = 0; i < dh.n_elements; i++)
		{
			fscanf(ein, "%07d %07d %02d %02d %02d %02d %4d %02d %02d %4d", &dh.array_ptr[i].bnr, &dh.array_ptr[i].kdnr, &dh.array_ptr[i].kat, &dh.array_ptr[i].hausnr, &dh.array_ptr[i].anreise.tag, &dh.array_ptr[i].anreise.monat, &dh.array_ptr[i].anreise.jahr, &dh.array_ptr[i].abreise.tag, &dh.array_ptr[i].abreise.monat, &dh.array_ptr[i].abreise.jahr);
		}
		fclose(ein);
	}

	do
	{
		printf("\nAuswahl ... \n[A]nfrage \n[B]uchung \n[S]torno \n[U]ebersicht alle Buchungen \n[E]nde\n\n");
		ausw = eingabe_ausw();

		switch (ausw)
		{
		case 'a':
			anfrage(&dh, &anreise, &abreise, ts);
			break;
		case 'b':
			buchung(&dh, &anreise, &abreise, ts);
			break;
		case 's':
			storno(&dh);
			break;
		case 'u':
			anzeigen(dh);
			break;
		case 'e':
			printf("Das Programm wird beendet\n\n");

			aus2 = fopen(hilfsarray, "wt");
			if (aus2 == NULL)
				printf("Fehler beim Oeffnen/Anlegen der Datei %s.\n", hilfsarray);
			else
			{
				fprintf(aus2, "%d %d", dh.n_allocated, dh.n_elements);
				fclose(aus2);
			}

			aus = fopen(ausgabedatei, "wt");
			if (aus == NULL)
				printf("Fehler beim Oeffnen/Anlegen der Datei %s.\n", ausgabedatei);
			else
			{
				for (i = 0; i < dh.n_elements; i++)
				{
					//printf("%07d %07d %02d %02d %02d %02d %4d %02d %02d %4d\n", dh.array_ptr[i].bnr, dh.array_ptr[i].kdnr, dh.array_ptr[i].hausnr, dh.array_ptr[i].kat, dh.array_ptr[i].anreise.tag, dh.array_ptr[i].anreise.monat, dh.array_ptr[i].anreise.jahr, dh.array_ptr[i].abreise.tag, dh.array_ptr[i].abreise.monat, dh.array_ptr[i].abreise.jahr);
					fprintf(aus, "%07d %07d %02d %02d %02d %02d %4d %02d %02d %4d\n", dh.array_ptr[i].bnr, dh.array_ptr[i].kdnr, dh.array_ptr[i].hausnr, dh.array_ptr[i].kat, dh.array_ptr[i].anreise.tag, dh.array_ptr[i].anreise.monat, dh.array_ptr[i].anreise.jahr, dh.array_ptr[i].abreise.tag, dh.array_ptr[i].abreise.monat, dh.array_ptr[i].abreise.jahr);
				}
				fclose(aus);
			}

			free(dh.array_ptr);
			ende = 1;
			break;
		default:
			printf("Ungueltige Eingabe\n\n\a");
		}
	} while (ende == 0);

	return 0;
}
