<script lang="ts">
	import LandingPage from "$lib/components/LandingPage.svelte";
	let { data }: { data: any } = $props();
</script>

<LandingPage locale="en" version={data.version} />
