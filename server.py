#!/usr/bin/env python 

import flask
import json

app = flask.Flask(__name__)

@app.route("/")
def index():
    fields = json.load(open("/etc/brokenhub.conf", "rb"))
    return flask.render_template("base.html", config=fields)

@app.route("/set", methods=("POST", "GET"))
def set_config():
    VALID_FIELDS = frozenset(("drop_percent",
                              "corrupt_packet_percent",
                              "corrupt_packet_bytes",
                              "truncate_len",
                              "bandwidth",))
    fields = json.load(open("/etc/brokenhub.conf", "rb"))
    for key, value in flask.request.form.iteritems():
        assert key in VALID_FIELDS
        fields[key] = value
    with open("/etc/brokenhub.conf", "wb") as fh:
        json.dump(fields, fh)
    subprocess.check_call(["/etc/init.d/brokenhub", "reload"])
    return flask.redirect("/")


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=80)
