<script lang="ts">
	import { Search, FileText, Hash } from "@lucide/svelte";
	import type { Locale } from "$lib/docs/i18n";

	type Entry = {
		title: string;
		url: string;
		locale: Locale;
		headings: { text: string; id: string }[];
	};

	let { open = $bindable(false), locale }: { open?: boolean; locale: Locale } = $props();

	let index = $state<Entry[] | null>(null);
	let query = $state("");
	let selected = $state(0);
	let inputEl = $state<HTMLInputElement | null>(null);

	$effect(() => {
		if (open && !index) {
			fetch("/search-index.json")
				.then((r) => r.json())
				.then((data) => (index = data))
				.catch(() => (index = []));
		}
	});

	$effect(() => {
		if (open) inputEl?.focus();
		else query = "";
	});

	const results = $derived.by(() => {
		const q = query.trim().toLowerCase();
		if (!index || !q) return [];
		const out: { title: string; url: string; heading?: string }[] = [];
		for (const entry of index) {
			if (entry.locale !== locale) continue;
			if (entry.title.toLowerCase().includes(q)) {
				out.push({ title: entry.title, url: entry.url });
			}
			for (const h of entry.headings) {
				if (h.text.toLowerCase().includes(q)) {
					out.push({ title: entry.title, url: `${entry.url}#${h.id}`, heading: h.text });
				}
			}
			if (out.length >= 20) break;
		}
		return out.slice(0, 20);
	});

	$effect(() => {
		query;
		selected = 0;
	});

	function onkeydown(e: KeyboardEvent) {
		if ((e.metaKey || e.ctrlKey) && e.key === "k") {
			e.preventDefault();
			open = !open;
			return;
		}
		if (!open) return;
		if (e.key === "Escape") {
			open = false;
		} else if (e.key === "ArrowDown") {
			e.preventDefault();
			selected = Math.min(selected + 1, results.length - 1);
		} else if (e.key === "ArrowUp") {
			e.preventDefault();
			selected = Math.max(selected - 1, 0);
		} else if (e.key === "Enter" && results[selected]) {
			go(results[selected].url);
		}
	}

	function go(url: string) {
		open = false;
		window.location.href = url;
	}
</script>

<svelte:window {onkeydown} />

{#if open}
	<div class="fixed inset-0 z-[60]">
		<button
			class="absolute inset-0 bg-black/50"
			onclick={() => (open = false)}
			aria-label={locale === "id" ? "Tutup pencarian" : "Close search"}
		></button>
		<div
			class="absolute left-1/2 top-24 w-[calc(100%-2rem)] max-w-lg -translate-x-1/2 overflow-hidden rounded-lg border border-border bg-popover text-popover-foreground shadow-lg"
			role="dialog"
			aria-label={locale === "id" ? "Cari" : "Search"}
		>
			<div class="flex items-center border-b border-border px-3">
				<Search size={16} class="mr-2 shrink-0 opacity-50" aria-hidden="true" />
				<input
					type="text"
					bind:this={inputEl}
					bind:value={query}
					autocomplete="off"
					autocorrect="off"
					spellcheck="false"
					placeholder={locale === "id" ? "Cari dokumentasi..." : "Search docs..."}
					class="h-11 w-full bg-transparent text-sm outline-none placeholder:text-muted-foreground"
				/>
			</div>
			<div class="max-h-72 overflow-y-auto p-1">
				{#if query.trim()}
					{#if results.length === 0}
						<div class="py-6 text-center text-sm text-muted-foreground">
							{locale === "id" ? "Tidak ada hasil." : "No results found."}
						</div>
					{:else}
						{#each results as r, i (r.url)}
							<button
								onclick={() => go(r.url)}
								onmouseenter={() => (selected = i)}
								class="flex w-full items-center gap-2 rounded-md px-2 py-2 text-left text-sm {i ===
								selected
									? 'bg-accent text-accent-foreground'
									: ''} {r.heading ? 'pl-8' : ''}"
							>
								{#if r.heading}
									<Hash size={14} class="shrink-0 opacity-60" aria-hidden="true" />
									<span class="truncate">{r.heading}</span>
									<span class="ml-auto shrink-0 truncate text-xs text-muted-foreground"
										>{r.title}</span
									>
								{:else}
									<FileText size={16} class="shrink-0 opacity-60" aria-hidden="true" />
									<span class="truncate">{r.title}</span>
								{/if}
							</button>
						{/each}
					{/if}
				{:else}
					<div class="py-6 text-center text-sm text-muted-foreground">
						{locale === "id"
							? "Ketik untuk mencari halaman dan bagian..."
							: "Type to search pages and sections..."}
					</div>
				{/if}
			</div>
		</div>
	</div>
{/if}
