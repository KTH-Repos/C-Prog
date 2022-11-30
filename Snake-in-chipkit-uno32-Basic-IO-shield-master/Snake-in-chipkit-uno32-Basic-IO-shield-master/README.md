# Snake

### Mål

Målet med projektet är att bygga ett en variant av spelet Snake på ChipKIT med vissagrundläggande egenskaper såsom att ormen ska kunna äta äpplen (dvs. få poäng), kunnaflytta runt på skärmen och kollidera (förlora). Vi tänker också implementera olika hinder(föremål) som kommer fram beroende på spelarens poäng och att spelarens hastighet ökar. Vitänker till en början följa dessa krav, men ifall vi har tid över kommer vi att göra projektetmer avancerat.


### Krav

- [ ] Ormen styrs med hjälp av knapparna på ChipKIT
- [ ] Äpplen som ormen kan äta (få poäng)
- [ ] Skriva ut poängen på skärmen
- [ ] Hastigheten ändras beroende på spelarens poäng

### Lösning

Vi kommer att utveckla vårt projekt på ChipKIT Uno32 tillsammans med Basic I/O shield. Vikommer att använda skärmen från Basic I/O shield för att visa spelet. Spelaren kommer attstyra ormen med hjälp av knapparna på Basic I/O shield. Vi kommer att använda interruptsför att kontrollera tiden, hastigheten och uppdatera skärmen. Koden kommer att skrivas i C.

