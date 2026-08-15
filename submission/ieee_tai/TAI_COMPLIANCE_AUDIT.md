# IEEE TAI Compliance Audit Report (`TAI_COMPLIANCE_AUDIT.md`)
**Manuscript ID**: `TAI-2026-Aug-A-01875`  
**Manuscript Title**: *AdaptiveReplica: Dynamic Quorum Adaptation and Failure-Aware Replica Selection in Distributed Consensus*

---

## 1. Compliance Audit & Comparison Against `TAI_Word_Template.doc`

| Audit Dimension / Requirement | Official Template Evidence (`TAI_Word_Template.doc`) | Previous Manuscript State | Status | Required Correction | File Affected |
| :--- | :--- | :--- | :---: | :--- | :--- |
| **Document Layout & Columns** | 2-column format with 0.5–0.75 in margins, section breaks | Custom single-column format generated via generic docx script | **FAIL** | Populate the actual `TAI_Word_Template.doc` directly using native 2-column IEEE styles | `AdaptiveReplica_TAI_Main_Manuscript.docx` |
| **Native Typography Styles** | Built-in styles: `Title`, `Authors`, `Abstract`, `IndexTerms`, `Heading 1`, `Text` | Generic Arial font styles with manual underscore dividers | **FAIL** | Apply exact native `TAI_Word_Template.doc` paragraph styles without ad-hoc formatting | `AdaptiveReplica_TAI_Main_Manuscript.docx` |
| **Impact Statement** | Explicit `Impact Statement` subsection ($\le 150$ words) | Present (105 words) | **PASS** | Retain exact 105-word Impact Statement in template section | `AdaptiveReplica_TAI_Main_Manuscript.docx`, `main.tex` |
| **Abstract Structure** | Single paragraph, 150–250 words, self-contained, no equations/citations | 151 words, 1 paragraph, self-contained | **PASS** | Retain exact 151-word verified abstract | `AdaptiveReplica_TAI_Main_Manuscript.docx`, `main.tex` |
| **Index Terms** | Alphabetical order, separated by commas, beginning with `Index Terms—` | Present, 8 terms | **PASS** | Format with native `IndexTerms` style | `AdaptiveReplica_TAI_Main_Manuscript.docx`, `main.tex` |
| **Double-Anonymous Anonymity** | Anonymized review document with author names removed | Previous `references.bib` contained a self-citation (`thakare2026quorumshift`) | **FAIL** | Remove `thakare2026quorumshift` self-citation to ensure 100% blind review compliance | `references.bib`, `main.tex` |
| **Separate Title Page** | Author details, email, affiliations, and acknowledgments in separate `.docx` | Created previously, but needed email update to `shamthakare3000@gmail.com` | **NEEDS CORRECTION** | Regenerate `AdaptiveReplica_TAI_Title_Page.docx` with updated contact email and AI disclosure | `AdaptiveReplica_TAI_Title_Page.docx` |
| **Equations & Math** | Numbered consecutively `(1)`, `(2)`, right-aligned | Equations present, correctly referenced | **PASS** | Preserve mathematical notation and invariant proofs | `AdaptiveReplica_TAI_Main_Manuscript.docx`, `main.tex` |
| **Tables & Figures** | Table captions centered with Roman/Arabic numerals; native vector figure | Table I present; vector latency plot present | **PASS** | Format Table I in template style; embed high-resolution plot | `AdaptiveReplica_TAI_Main_Manuscript.docx`, `main.tex` |
| **Bibliographic Integrity** | Numbered IEEE citations `[1]–[9]` matching verified literature | 9 verified classical distributed systems citations | **PASS** | Retain 9 verified citations (`ongaro2014search`, `lamport1998part`, etc.) | `references.bib` |

---

## 2. Root Cause of Previous Unsubmission

1. **Failure to Use the Native TAI Word Template**: The previous Word document upload was generated programmatically as a single-column blank `.docx` file rather than inheriting the official multi-column IEEE TAI template (`TAI_Word_Template.doc`).
2. **Self-Citation Violation in References**: The old `references.bib` included an unblinded self-citation (`thakare2026quorumshift`), compromising double-blind peer review.
3. **Template Style Non-Conformity**: Lack of native IEEE character/paragraph style mappings (`Heading 1`, `IndexTerms`, `Abstract`).
