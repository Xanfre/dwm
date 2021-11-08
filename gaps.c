/* Key binding functions */
static void defaultgaps(const Arg *arg);
static void incrgaps(const Arg *arg);
static void togglegaps(const Arg *arg);
/* Layouts (delete the ones you do not need) */
static void bstack(Monitor *m);
static void centeredmaster(Monitor *m);
static void deck(Monitor *m);
static void dwindle(Monitor *m);
static void fibonacci(Monitor *m, int s);
static void spiral(Monitor *m);
static void tile(Monitor *m);
/* Internals */
static void getgaps(Monitor *m, int *g, unsigned int *nc);
static void getfacts(Monitor *m, int msize, int ssize, float *mf, float *sf, int *mr, int *sr);
static void setgaps(int g);

void
setgaps(int g)
{
	if (g < 0) g = 0;

	selmon->gap = g;
	arrange(selmon);
}

void
togglegaps(const Arg *arg)
{
	selmon->enablegaps = !selmon->enablegaps;
	arrange(selmon);
}

void
defaultgaps(const Arg *arg)
{
	setgaps(gap);
}

void
incrgaps(const Arg *arg)
{
	setgaps(selmon->gap + arg->i);
}

void
getgaps(Monitor *m, int *g, unsigned int *nc)
{
	unsigned int n, e;
	e = selmon->enablegaps;
	Client *c;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);

	*g = m->gap*e;      // gap
	*nc = n;            // number of clients
}

void
getfacts(Monitor *m, int msize, int ssize, float *mf, float *sf, int *mr, int *sr)
{
	unsigned int n;
	float mfacts, sfacts;
	int mtotal = 0, stotal = 0;
	Client *c;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);
	mfacts = MIN(n, m->nmaster);
	sfacts = n - m->nmaster;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++)
		if (n < m->nmaster)
			mtotal += msize / mfacts;
		else
			stotal += ssize / sfacts;

	*mf = mfacts; // total factor of master area
	*sf = sfacts; // total factor of stack area
	*mr = msize - mtotal; // the remainder (rest) of pixels after an even master split
	*sr = ssize - stotal; // the remainder (rest) of pixels after an even stack split
}

/***
 * Layouts
 */

/*
 * Bottomstack layout + gaps
 * https://dwm.suckless.org/patches/bottomstack/
 */
static void
bstack(Monitor *m)
{
	unsigned int i, n;
	int g;
	int mx = 0, my = 0, mh = 0, mw = 0;
	int sx = 0, sy = 0, sh = 0, sw = 0;
	float mfacts, sfacts;
	int mrest, srest;
	Client *c;

	getgaps(m, &g, &n);
	if (n == 0)
		return;

	sx = mx = m->wx + g;
	sy = my = m->wy + g;
	sh = mh = m->wh - 2*g;
	mw = m->ww - 2*g - g * (MIN(n, m->nmaster) - 1);
	sw = m->ww - 2*g - g * (n - m->nmaster - 1);

	if (m->nmaster && n > m->nmaster) {
		sh = (mh - g) * (1 - m->mfact);
		mh = mh - g - sh;
		sx = mx;
		sy = my + mh + g;
	}

	getfacts(m, mw, sw, &mfacts, &sfacts, &mrest, &srest);

	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
		if (i < m->nmaster) {
			resize(c, mx, my, (mw / mfacts) + (i < mrest ? 1 : 0) - (2*c->bw), mh - (2*c->bw), 0);
			mx += WIDTH(c) + g;
		} else {
			resize(c, sx, sy, (sw / sfacts) + ((i - m->nmaster) < srest ? 1 : 0) - (2*c->bw), sh - (2*c->bw), 0);
			sx += WIDTH(c) + g;
		}
	}
}

/*
 * Centred master layout + gaps
 * https://dwm.suckless.org/patches/centeredmaster/
 */
void
centeredmaster(Monitor *m)
{
	unsigned int i, n;
	int g;
	int mx = 0, my = 0, mh = 0, mw = 0;
	int lx = 0, ly = 0, lw = 0, lh = 0;
	int rx = 0, ry = 0, rw = 0, rh = 0;
	float mfacts = 0, lfacts = 0, rfacts = 0;
	int mtotal = 0, ltotal = 0, rtotal = 0;
	int mrest = 0, lrest = 0, rrest = 0;
	Client *c;

	getgaps(m, &g, &n);
	if (n == 0)
		return;

	/* initialize areas */
	mx = m->wx + g;
	my = m->wy + g;
	mh = m->wh - 2*g - g * ((!m->nmaster ? n : MIN(n, m->nmaster)) - 1);
	mw = m->ww - 2*g;
	lh = m->wh - 2*g - g * (((n - m->nmaster) / 2) - 1);
	rh = m->wh - 2*g - g * (((n - m->nmaster) / 2) - ((n - m->nmaster) % 2 ? 0 : 1));

	if (m->nmaster && n > m->nmaster) {
		/* go mfact box in the center if more than nmaster clients */
		if (n - m->nmaster > 1) {
			/* ||<-S->|<---M--->|<-S->|| */
			mw = (m->ww - 4*g) * m->mfact;
			lw = (m->ww - mw - 4*g) / 2;
			rw = (m->ww - mw - 4*g) - lw;
			mx += lw + g;
		} else {
			/* ||<---M--->|<-S->|| */
			mw = (mw - g) * m->mfact;
			lw = 0;
			rw = m->ww - mw - 3*g;
		}
		lx = m->wx + g;
		ly = m->wy + g;
		rx = mx + mw + g;
		ry = m->wy + g;
	}

	/* calculate facts */
	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++) {
		if (!m->nmaster || n < m->nmaster)
			mfacts += 1;
		else if ((n - m->nmaster) % 2)
			lfacts += 1; // total factor of left hand stack area
		else
			rfacts += 1; // total factor of right hand stack area
	}

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++)
		if (!m->nmaster || n < m->nmaster)
			mtotal += mh / mfacts;
		else if ((n - m->nmaster) % 2)
			ltotal += lh / lfacts;
		else
			rtotal += rh / rfacts;

	mrest = mh - mtotal;
	lrest = lh - ltotal;
	rrest = rh - rtotal;

	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
		if (!m->nmaster || i < m->nmaster) {
			/* nmaster clients are stacked vertically, in the center of the screen */
			resize(c, mx, my, mw - (2*c->bw), (mh / mfacts) + (i < mrest ? 1 : 0) - (2*c->bw), 0);
			my += HEIGHT(c) + g;
		} else {
			/* stack clients are stacked vertically */
			if ((i - m->nmaster) % 2 ) {
				resize(c, lx, ly, lw - (2*c->bw), (lh / lfacts) + ((i - 2*m->nmaster) < 2*lrest ? 1 : 0) - (2*c->bw), 0);
				ly += HEIGHT(c) + g;
			} else {
				resize(c, rx, ry, rw - (2*c->bw), (rh / rfacts) + ((i - 2*m->nmaster) < 2*rrest ? 1 : 0) - (2*c->bw), 0);
				ry += HEIGHT(c) + g;
			}
		}
	}
}

/*
 * Deck layout + gaps
 * https://dwm.suckless.org/patches/deck/
 */
void
deck(Monitor *m)
{
	unsigned int i, n;
	int g;
	int mx = 0, my = 0, mh = 0, mw = 0;
	int sx = 0, sy = 0, sh = 0, sw = 0;
	float mfacts, sfacts;
	int mrest, srest;
	Client *c;

	getgaps(m, &g, &n);
	if (n == 0)
		return;

	sx = mx = m->wx + g;
	sy = my = m->wy + g;
	sh = mh = m->wh - 2*g - g * (MIN(n, m->nmaster) - 1);
	sw = mw = m->ww - 2*g;

	if (m->nmaster && n > m->nmaster) {
		sw = (mw - g) * (1 - m->mfact);
		mw = mw - g - sw;
		sx = mx + mw + g;
		sh = m->wh - 2*g;
	}

	getfacts(m, mh, sh, &mfacts, &sfacts, &mrest, &srest);

	if (n - m->nmaster > 0) /* override layout symbol */
		snprintf(m->ltsymbol, sizeof m->ltsymbol, "D %d", n - m->nmaster);

	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++)
		if (i < m->nmaster) {
			resize(c, mx, my, mw - (2*c->bw), (mh / mfacts) + (i < mrest ? 1 : 0) - (2*c->bw), 0);
			my += HEIGHT(c) + g;
		} else {
			resize(c, sx, sy, sw - (2*c->bw), sh - (2*c->bw), 0);
		}
}

/*
 * Fibonacci layout + gaps
 * https://dwm.suckless.org/patches/fibonacci/
 */
void
fibonacci(Monitor *m, int s)
{
	unsigned int i, n, nx, ny, nw, nh;
	int g;
	int nv, hrest = 0, wrest = 0, r = 1;
	Client *c;

	getgaps(m, &g, &n);
	if (n == 0)
		return;

	nx = m->wx + g;
	ny = m->wy + g;
	nw = m->ww - 2*g;
	nh = m->wh - 2*g;

	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next)) {
		if (r) {
			if ((i % 2 && (nh - g) / 2 <= (bh + 2*c->bw))
			   || (!(i % 2) && (nw - g) / 2 <= (bh + 2*c->bw))) {
				r = 0;
			}
			if (r && i < n - 1) {
				if (i % 2) {
					nv = (nh - g) / 2;
					hrest = nh - 2*nv - g;
					nh = nv;
				} else {
					nv = (nw - g) / 2;
					wrest = nw - 2*nv - g;
					nw = nv;
				}

				if ((i % 4) == 2 && !s)
					nx += nw + g;
				else if ((i % 4) == 3 && !s)
					ny += nh + g;
			}

			if ((i % 4) == 0) {
				if (s) {
					ny += nh + g;
					nh += hrest;
				}
				else {
					nh -= hrest;
					ny -= nh + g;
				}
			}
			else if ((i % 4) == 1) {
				nx += nw + g;
				nw += wrest;
			}
			else if ((i % 4) == 2) {
				ny += nh + g;
				nh += hrest;
				if (i < n - 1)
					nw += wrest;
			}
			else if ((i % 4) == 3) {
				if (s) {
					nx += nw + g;
					nw -= wrest;
				} else {
					nw -= wrest;
					nx -= nw + g;
					nh += hrest;
				}
			}
			if (i == 0)	{
				if (n != 1) {
					nw = (m->ww - 3*g) - (m->ww - 3*g) * (1 - m->mfact);
					wrest = 0;
				}
				ny = m->wy + g;
			}
			else if (i == 1)
				nw = m->ww - nw - 3*g;
			i++;
		}

		resize(c, nx, ny, nw - (2*c->bw), nh - (2*c->bw), False);
	}
}

void
dwindle(Monitor *m)
{
	fibonacci(m, 1);
}

void
spiral(Monitor *m)
{
	fibonacci(m, 0);
}
