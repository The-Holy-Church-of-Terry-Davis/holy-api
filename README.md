# holy-api

hello, ok so...
this is a simple api inspired by Terry himself.

THE API IS LOCATED AT: https://api.thcotd.repl.co

`www` will host the endpoints and what not.

`./compile` compiles the project into a single executable: `./api`
`./api` runs it.

## Endpoints:
```
    /                 :  home.
    /status           :  if the API is up, it returns `OK`.
    /info             :  shows some information about the API and who THCoTD is.
    /vocab            :  shows `vocab.txt`.
    /godsay           :  uhhh, godsay. `?amount=10` defines how many words.
    /quote            :  returns a quote from Terry. `?amount=10` defines how many quotes.
    /divineintellect  :  returns how much divine intellect you have!
    /askterry         :  ask Terry a yes/no question and he'll respond!
```
