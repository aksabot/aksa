<script lang="ts">
	import TableOfContents from "$lib/components/TableOfContents.svelte";

	let { data }: { data: any } = $props();
</script>

<div class="flex gap-8">
	<div class="min-w-0 flex-1">
		<!-- Page header -->
		<div class="mb-6">
			<h1 class="text-3xl font-bold tracking-tight">{data.frontmatter.title}</h1>
			{#if data.frontmatter.description}
				<p class="mt-2 text-lg text-muted-foreground">{data.frontmatter.description}</p>
			{/if}
		</div>

		<!-- Rendered markdown -->
		<div class="prose">
			{@html data.html}
		</div>
	</div>

	<!-- Table of Contents (desktop) -->
	<aside class="hidden w-48 shrink-0 xl:block">
		<div class="sticky top-16 py-6">
			<TableOfContents headings={data.headings} locale={data.locale} />
		</div>
	</aside>
</div>
