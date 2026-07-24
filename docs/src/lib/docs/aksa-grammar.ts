// Syntax grammar for Aksa code blocks. Keywords and builtins cover every
// locale (en + id) so one grammar highlights examples on both language pages.
// Planned hardware builtins (atur, sudut, baca_jarak) are included so
// upcoming-feature docs highlight correctly.

const KEYWORDS = [
	"if",
	"else",
	"repeat",
	"while",
	"function",
	"return",
	"and",
	"or",
	"not",
	"make",
	"stop",
	"jika",
	"lainnya",
	"ulangi",
	"selama",
	"fungsi",
	"kembali",
	"dan",
	"atau",
	"bukan",
	"buat",
	"berhenti",
];

const CONSTANTS = ["true", "false", "benar", "salah"];

const BUILTINS = [
	"print",
	"ask",
	"forward",
	"backward",
	"turn_right",
	"turn_left",
	"color",
	"pen_up",
	"pen_down",
	"turn_on",
	"turn_off",
	"read",
	"read_analog",
	"wait",
	"cetak",
	"tanya",
	"maju",
	"mundur",
	"belok_kanan",
	"belok_kiri",
	"warna",
	"angkat_pena",
	"turunkan_pena",
	"nyalakan",
	"matikan",
	"baca",
	"baca_analog",
	"tunggu",
	"atur",
	"sudut",
	"baca_jarak",
];

export const aksaGrammar = {
	name: "aksa",
	scopeName: "source.aksa",
	patterns: [
		{ include: "#comment" },
		{ include: "#string" },
		{ include: "#number" },
		{ include: "#constant" },
		{ include: "#keyword" },
		{ include: "#builtin" },
	],
	repository: {
		comment: { match: "//.*$", name: "comment.line.double-slash.aksa" },
		string: { begin: '"', end: '"', name: "string.quoted.double.aksa" },
		number: { match: "\\b\\d+(\\.\\d+)?\\b", name: "constant.numeric.aksa" },
		constant: { match: `\\b(${CONSTANTS.join("|")})\\b`, name: "constant.language.aksa" },
		keyword: { match: `\\b(${KEYWORDS.join("|")})\\b`, name: "keyword.control.aksa" },
		builtin: { match: `\\b(${BUILTINS.join("|")})\\b`, name: "entity.name.function.aksa" },
	},
};
