export interface NavItem {
	label: string;
	labelId?: string;
	slug?: string;
	children?: NavItem[];
}

export interface NavGroup {
	label: string;
	labelId?: string;
	items: NavItem[];
}

export const sidebar: NavGroup[] = [
	{
		label: "Start Here",
		labelId: "Mulai di Sini",
		items: [
			{ label: "Introduction", labelId: "Pengenalan", slug: "" },
			{ label: "Getting Started", labelId: "Memulai", slug: "getting-started" },
		],
	},
	{
		label: "Language",
		labelId: "Bahasa",
		items: [
			{ label: "Overview", labelId: "Ringkasan", slug: "language/overview" },
			{ label: "Keywords", labelId: "Kata Kunci", slug: "language/keywords" },
			{ label: "Types & Values", labelId: "Tipe & Nilai", slug: "language/types" },
			{ label: "Control Flow", labelId: "Alur Kendali", slug: "language/control-flow" },
			{ label: "Functions", labelId: "Fungsi", slug: "language/functions" },
			{ label: "Grammar", labelId: "Tata Bahasa", slug: "language/grammar" },
		],
	},
	{
		label: "Learn",
		labelId: "Belajar",
		items: [
			{
				label: "Basic",
				labelId: "Dasar",
				children: [
					{ label: "1. Writing", labelId: "1. Menulis", slug: "learn/lesson-01" },
					{ label: "2. Variables", labelId: "2. Variabel", slug: "learn/lesson-02" },
					{ label: "3. Math", labelId: "3. Berhitung", slug: "learn/lesson-03" },
					{ label: "4. Asking", labelId: "4. Bertanya", slug: "learn/lesson-04" },
					{ label: "5. Repeating", labelId: "5. Mengulang", slug: "learn/lesson-05" },
					{ label: "6. Choosing", labelId: "6. Memilih", slug: "learn/lesson-06" },
				],
			},
			{
				label: "Middle",
				labelId: "Menengah",
				children: [
					{ label: "7. Counting", labelId: "7. Menghitung", slug: "learn/lesson-07" },
					{ label: "8. While", labelId: "8. Selama", slug: "learn/lesson-08" },
					{ label: "9. True & False", labelId: "9. Benar & Salah", slug: "learn/lesson-09" },
					{ label: "10. Functions", labelId: "10. Fungsi", slug: "learn/lesson-10" },
					{ label: "11. Returning Answers", labelId: "11. Jawaban Fungsi", slug: "learn/lesson-11" },
				],
			},
			{
				label: "Advanced",
				labelId: "Lanjutan",
				children: [
					{ label: "12. Even or Odd", labelId: "12. Genap atau Ganjil", slug: "learn/lesson-12" },
					{ label: "13. Robot", labelId: "13. Robot", slug: "learn/lesson-13" },
					{ label: "14. Project: Spiral", labelId: "14. Proyek: Spiral", slug: "learn/lesson-14" },
					{
						label: "15. Project: Guess the Number",
						labelId: "15. Proyek: Tebak Angka",
						slug: "learn/lesson-15",
					},
				],
			},
		],
	},
	{
		label: "Aksabot",
		items: [
			{ label: "Overview", labelId: "Ringkasan", slug: "aksabot/overview" },
			{ label: "Builtins", labelId: "Perintah Bawaan", slug: "aksabot/builtins" },
		],
	},
	{
		label: "Reference",
		labelId: "Referensi",
		items: [
			{ label: "Builtins", labelId: "Perintah Bawaan", slug: "reference/builtins" },
			{ label: "Error Codes", labelId: "Kode Error", slug: "reference/errors" },
			{ label: "CLI", slug: "reference/cli" },
			{ label: "Changelog", slug: "reference/changelog" },
			{ label: "Roadmap", slug: "reference/roadmap" },
		],
	},
];
