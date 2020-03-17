<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="text"/>

    <xsl:template match="/">% This is a generated file -- don't modify it manually.
\NeedsTeXFormat{LaTeX2e}
\ProvidesPackage{dvisvgm}[2015/02/04 dvisvgm DocBook style]
\RequirePackageWithOptions{docbook}
\RequirePackage[english]{babel}
\RequirePackage[shortcuts]{extdash}
\RequirePackage{underscore}
\renewcommand*{\arraystretch}{-0.4}
\AtBeginDocument{%
  \thispagestyle{empty}
  \lhead[]{\refmiscinfomanual}
  \rhead[]{\thepage}
  \lfoot[]{\refmiscinfosource{} \refmiscinfoversion}
  \rfoot[]{<xsl:value-of select="refentry/refentryinfo/date"/>} % revision date given in dvisvgm.txt.in
  \def\tableofcontents{\stdtoc} % keep TOC on current page
}
    </xsl:template>
</xsl:stylesheet>
