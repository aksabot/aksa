<script lang="ts">
	import ThemeToggle from "./ThemeToggle.svelte";
	import { Search } from "@lucide/svelte";
	import type { Locale } from "$lib/docs/i18n";

	let {
		version = "",
		locale,
		switchLocaleUrl,
		onMenuToggle,
		onSearch,
	}: {
		version?: string;
		locale: Locale;
		switchLocaleUrl: string;
		onMenuToggle?: () => void;
		onSearch?: () => void;
	} = $props();
</script>

<header
	class="sticky top-0 z-50 border-b border-border bg-background/95 backdrop-blur supports-[backdrop-filter]:bg-background/60"
>
	<div class="mx-auto flex h-14 max-w-7xl items-center gap-4 px-4 sm:px-6">
		<!-- Mobile menu button -->
		{#if onMenuToggle}
			<button
				onclick={onMenuToggle}
				class="inline-flex h-8 w-8 items-center justify-center rounded-md text-muted-foreground hover:bg-accent hover:text-foreground lg:hidden"
				aria-label="Toggle menu"
			>
				<svg
					xmlns="http://www.w3.org/2000/svg"
					width="18"
					height="18"
					viewBox="0 0 24 24"
					fill="none"
					stroke="currentColor"
					stroke-width="2"
					stroke-linecap="round"
					stroke-linejoin="round"
					><line x1="4" x2="20" y1="12" y2="12" /><line x1="4" x2="20" y1="6" y2="6" /><line
						x1="4"
						x2="20"
						y1="18"
						y2="18"
					/></svg
				>
			</button>
		{/if}

		<!-- Logo -->
		<a href={locale === "id" ? "/id" : "/"} class="flex items-center gap-2 font-semibold">
			<img src="/logo-dark.svg" alt="Aksa" class="hidden h-6 w-6 dark:block" />
			<img src="/logo-light.svg" alt="Aksa" class="block h-6 w-6 dark:hidden" />
			<span>Aksa</span>
			{#if version}
				<span class="rounded-md bg-muted px-1.5 py-0.5 text-xs text-muted-foreground"
					>{version}</span
				>
			{/if}
		</a>

		<div class="flex-1"></div>

		<!-- Right actions -->
		<nav class="flex items-center gap-1">
			<!-- Search -->
			{#if onSearch}
				<button
					onclick={onSearch}
					class="inline-flex h-8 items-center gap-2 rounded-md px-2 text-xs font-medium text-muted-foreground transition-colors hover:bg-accent hover:text-foreground sm:border sm:border-border"
					aria-label={locale === "id" ? "Cari dokumentasi" : "Search docs"}
				>
					<Search size={14} />
					<span class="hidden sm:inline">{locale === "id" ? "Cari..." : "Search..."}</span>
					<kbd
						class="hidden rounded bg-muted px-1.5 py-0.5 font-mono text-[10px] text-muted-foreground sm:inline"
						>⌘K</kbd
					>
				</button>
			{/if}

			<!-- Language switcher -->
			<a
				href={switchLocaleUrl}
				class="inline-flex h-8 items-center rounded-md px-2 text-xs font-medium text-muted-foreground transition-colors hover:bg-accent hover:text-foreground"
			>
				{locale === "en" ? "ID" : "EN"}
			</a>

			<ThemeToggle />

			<!-- Playground -->
			<a
				href="https://play.aksabot.com"
				target="_blank"
				rel="noopener noreferrer"
				class="inline-flex h-8 items-center rounded-md bg-primary px-3 text-xs font-medium text-primary-foreground transition-opacity hover:opacity-90"
			>
				{locale === "id" ? "▶ Main" : "▶ Play"}
			</a>

			<!-- Main site -->
			<a
				href="https://aksabot.com"
				target="_blank"
				rel="noopener noreferrer"
				class="hidden h-8 items-center rounded-md px-2 text-xs font-medium text-muted-foreground transition-colors hover:bg-accent hover:text-foreground sm:inline-flex"
			>
				aksabot.com
			</a>
		</nav>
	</div>
</header>
