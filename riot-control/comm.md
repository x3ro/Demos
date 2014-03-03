
# Communication Protocols and data exchange formats

## RIOT-Control -> Portal

## Portal -> RIOT-Control


## RIOT-Control -> Browser (via web-socket)

### Init

send once when a new client connects

```
channel: 'init'
{
    ... parsed JSON layout file ...
}
```

### Event

send for every event that happens

```
channel: 'event'
{
    'id': xx,       // event id
    'data': xx,     // event data
    'src': xx,      // event src
}
```

## Browser -> RIOT-Control (via web-socket)

### Event

the browser client can also create events

```
channel: 'event'
{
    'id': xx,       // event id
    'data': xx,     // event data
    'src': xx,      // event source address (filled by RIOT-Control)
}
```

### Reset

The client can make the host flush its RPL data

```
channel: 'reset'
{
    no data
}
```