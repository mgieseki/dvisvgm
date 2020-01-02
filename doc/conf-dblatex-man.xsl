<?xml version="1.0" encoding="UTF-8"?>
<!-- This file is part of dvisvgm -->
<!-- Copyright (C) 2015-2020 Martin Gieseking <martin.gieseking@uos.de> -->
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
	<!-- output monospaced text in bold -->
	<xsl:template match="literal">
		<xsl:text>\fB</xsl:text>
		<xsl:apply-templates/>
		<xsl:text>\fR</xsl:text>
	</xsl:template>

	<!-- append URL to hyperlinked text pointing to external targets -->
	<xsl:template match="ulink[not(contains(., '://')) and not(contains(@url, 'mailto:'))]">
		<xsl:apply-templates/>
		<xsl:text> (\m[blue]</xsl:text>
		<xsl:value-of select="@url"/>
		<xsl:text>\m[])</xsl:text>
	</xsl:template>
</xsl:stylesheet>
