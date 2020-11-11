const express = require('express');
const app = new express();
const fs = require('fs');

app.get('/', (req, res) => {
    const html = fs.readFileSync('/elements/procs/modram', 'utf8');
    var text = html.toString();
    res.send(text);
});

app.listen(process.env.port || 8080);
