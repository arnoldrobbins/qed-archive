# QED For Unix - 1992

# Introduction

The files in this directory are from a tarball sent to me by Rob Pike
(r AT golang.org),
one of the original developers of QED at the University of Toronto.
The files are dated September 7, 1992.  I chose not to try to layer
them on top of the files I have from 1985, but simply to present
them as I received them.

Of note is that there is more documentation here than in my tarball.

## Some More History

In January of 2021, Rob Pike supplied more history. Reprinted here
by permission.

> From: Rob Pike <robpike AT gmail.com>
> Date: Sun, 31 Jan 2021 19:29:46 +1100
> Message-ID: <CAKzdPgxMiFVt5ejZm4-AHKEmqnimpZvqZXKy2afCsvkTOw0NXA@mail.gmail.com>
> To: Lars Brinkhoff <lars AT nocrew.org>
> Subject: Re: [TUHS] Qed vs ed
> Cc: TUHS main list <tuhs AT minnie.tuhs.org>
>
> The version of qed Leah refers to is not pure. It was created in the late
> 1970s by Tom Duff, Hugh Redelmeier, David Tilbrook and myself by hacking
> the (already hacked) v6 ed we had at the University of Toronto, to restore
> some of the programmability that had been removed when ed was created, and
> to have fun. Mostly to have fun.
>
> Tom Duff used a real QED (sic) on the GCOS at Waterloo and was a bit
> conflicted about all this. For me the programmability was fun, and I wrote
> the tutorial, but what ended up sticking with me was the ability to edit
> multiple files simultaneously, something no other editor I had available
> could do. I don't mean switching between files, I mean making things like
> global substitutions across *.c. I'd start a session by typing qed *.[ch]
> and go from there.
>
> Our qed traveled with me from Caltech to Bell Labs, where I used it to
> write jim, which I used to write sam, which I used to write Acme. I tried
> vi when starting jim, but again the one file thing was too much to bear. I
> also tried emacs, which could in principle handle multiple files but the
> interface was cumbersome - it was much too hard to open a new file in a
> subwindow - and without regular expressions I gave up after a day or two.
> Also with a 2-d screen and a 1-d input device (no mouse), vi and emacs were
> too remote, like giving directions to someone holding a map without being
> able to point at the map. Describing where you want to point rather than
> just pointing.
>
> Anyway that was 40 or so years ago, and it's clear from the screens on
> others' desks at work that my opinion on those matters is not widely shared.
>
> I had a lot of fun hacking qed, mostly side-by-side with Tilbrook, who was
> its biggest fan. He taught me a lot and I loved working with him.
>
> -rob

#### Last Updated

Mon Feb  1 08:22:02 IST 2021
