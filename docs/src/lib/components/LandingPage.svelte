<script lang="ts">
	import DocsNavbar from "./DocsNavbar.svelte";
	import type { Locale } from "$lib/docs/i18n";
	import { localizeUrl } from "$lib/docs/i18n";

	let { locale, version = "" }: { locale: Locale; version?: string } = $props();

	const t = (en: string, id: string) => (locale === "id" ? id : en);

	const enCode = `function greet(name) {
  return "hello " + name
}

repeat 3 {
  print(greet("world"))
}`;

	const idCode = `fungsi sapa(nama) {
  kembali "halo " + nama
}

ulangi 3 {
  cetak(sapa("dunia"))
}`;
</script>

<div class="min-h-screen">
	<DocsNavbar {version} {locale} switchLocaleUrl={locale === "id" ? "/" : "/id"} />

	<main class="mx-auto max-w-5xl px-4 sm:px-6">
		<!-- Hero -->
		<section class="flex flex-col items-center gap-5 py-16 text-center sm:py-24">
			<img src="/logo-light.svg" alt="" class="block h-16 w-16 dark:hidden" />
			<img src="/logo-dark.svg" alt="" class="hidden h-16 w-16 dark:block" />
			<h1 class="text-4xl font-bold tracking-tight sm:text-5xl">Aksa</h1>
			<p class="max-w-xl text-lg text-muted-foreground">
				{t(
					"A tiny programming language for kids — with keywords in their language.",
					"Bahasa pemrograman kecil untuk anak — dengan kata kunci dalam bahasa mereka sendiri.",
				)}
			</p>
			<div class="mt-2 flex flex-wrap justify-center gap-3">
				<a
					href={localizeUrl("getting-started", locale)}
					class="rounded-md bg-primary px-4 py-2 text-sm font-medium text-primary-foreground transition-colors hover:bg-primary/90"
				>
					{t("Get Started", "Mulai")}
				</a>
				<a
					href={localizeUrl("learn/lesson-01", locale)}
					class="rounded-md border border-border bg-secondary px-4 py-2 text-sm font-medium text-secondary-foreground transition-colors hover:bg-secondary/80"
				>
					{t("Lessons", "Pelajaran")}
				</a>
				<a
					href={localizeUrl("aksabot/overview", locale)}
					class="rounded-md border border-border px-4 py-2 text-sm font-medium text-muted-foreground transition-colors hover:bg-accent hover:text-foreground"
				>
					Aksabot
				</a>
			</div>
		</section>

		<!-- Same program, two languages -->
		<section class="grid gap-4 pb-16 sm:grid-cols-2">
			<div class="rounded-lg border border-border bg-card p-4">
				<p class="mb-2 text-xs font-semibold uppercase tracking-wider text-muted-foreground">
					{t("English keywords", "Kata kunci Inggris")}
				</p>
				<pre class="overflow-x-auto rounded-md bg-muted p-4 text-sm leading-relaxed"><code
						>{enCode}</code
					></pre>
			</div>
			<div class="rounded-lg border border-border bg-card p-4">
				<p class="mb-2 text-xs font-semibold uppercase tracking-wider text-muted-foreground">
					{t("Indonesian keywords", "Kata kunci Indonesia")}
				</p>
				<pre class="overflow-x-auto rounded-md bg-muted p-4 text-sm leading-relaxed"><code
						>{idCode}</code
					></pre>
			</div>
			<p class="text-center text-sm text-muted-foreground sm:col-span-2">
				{t(
					"The same program, the same language — only the locale changes.",
					"Program yang sama, bahasa yang sama — hanya lokalnya yang berbeda.",
				)}
			</p>
		</section>

		<!-- Three pillars -->
		<section class="grid gap-4 pb-24 sm:grid-cols-3">
			<div class="rounded-lg border border-border bg-card p-5">
				<h3 class="mb-1.5 font-semibold">{t("Their own language", "Bahasa mereka sendiri")}</h3>
				<p class="text-sm text-muted-foreground">
					{t(
						"All 13 keywords, every builtin, and every error message come from a locale dictionary.",
						"Semua 13 kata kunci, setiap perintah bawaan, dan setiap pesan error berasal dari kamus bahasa.",
					)}
				</p>
			</div>
			<div class="rounded-lg border border-border bg-card p-5">
				<h3 class="mb-1.5 font-semibold">{t("Friendly errors", "Error yang ramah")}</h3>
				<p class="text-sm text-muted-foreground">
					{t(
						"The whole program is checked before it runs, so kids see all their mistakes at once — never a stack trace.",
						"Seluruh program diperiksa sebelum dijalankan, jadi anak melihat semua kesalahannya sekaligus — tanpa pesan teknis.",
					)}
				</p>
			</div>
			<div class="rounded-lg border border-border bg-card p-5">
				<h3 class="mb-1.5 font-semibold">{t("Real hardware", "Perangkat sungguhan")}</h3>
				<p class="text-sm text-muted-foreground">
					{t(
						"Zero installation in the browser, then the same program runs on a real ESP32 board.",
						"Tanpa instalasi di browser, lalu program yang sama berjalan di papan ESP32 sungguhan.",
					)}
				</p>
			</div>
		</section>
	</main>
</div>
