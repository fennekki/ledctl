ledctl
======

A tiny software for controlling Leds on Linux. Originally made for my
Banana Pi, so the featureset might not suit you.

In C90, because apparently that's still the default.

Using ledctl is entirely equivalent to using a shell to
`echo`/`cat`/`dd` some data around.

Notes on unprivileged users
---------------------------

The device nodes under `/sys/class/leds` are owned by root and don't
usually have public read/write privileges! What do?

Well, first of all, I'd suggest against giving unprivileged users the
right to write to system leds at all. If you do this, they might be
able to control such things as indicator leds on a laptop, leading to
such things as recording data from your webcam without you ever being
notified! In a well-configured server environment this is unlikely to be
an issue, but then, what leds are you controlling, anyway?

If you must have an user in control of some leds, I'd suggest giving
only *specific* users the right to, and only for *specific* device
nodes. ledctl will not help you do this: it's merely a different
interface to `/sys/class/leds`.

### What about the suid bit ###

The short answer is "Don't."

The long answer is as follows:

Given a `ledctl` binary owned by root:root, I know of no particular way
an user could exploit it. It uses hardcoded paths, or ones extracted
directly from calls to get directory contents, so libc and kernel issues
notwithstanding, these should not be an issue. It also doesn't let you
write arbitrary data, only arbitrary numbers between two values (`ledctl
brightness`) and one of a set of possible triggers (`ledctl trigger`)
into files at those paths. Again, kernel issues apply.

However, this assumes *absolutely no exploits in ledctl, sysfs, libc or
the led drivers you're using*, as well as correct filesystem privileges
for not only `/sys/class/leds/*` but also the `ledctl` binary itself. If
any non-privileged user is able to edit it, they can write to *arbitrary
files*. Without a suid bit, this isn't a problem - they cannot write to
files they don't own and `ledctl` will present an eror message. With the
suid bit, however, an *unprivileged* attacker can *truncate arbitrary
files* and insert semi-arbitrary digits into them. The digits written
might be able to facilitate an attack of some kind, as well.

Put short: Don't suid `ledctl`. That's not an appropriate way to use it.
