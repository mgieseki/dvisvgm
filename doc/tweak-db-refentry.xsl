<?xml version="1.0"?>
<!-- This file is part of dvisvgm -->
<!-- Copyright (C) 2015-2019 Martin Gieseking <martin.gieseking@uos.de> -->
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:date="http://exslt.org/dates-and-times"
	extension-element-prefixes="date">

	<xsl:output doctype-public="-//OASIS//DTD DocBook XML V4.5//EN" doctype-system="http://www.oasis-open.org/docbook/xml/4.5/docbookx.dtd"/>

	<!-- add date to refmeta element -->
	<xsl:template match="refmeta">
		<xsl:copy>
			<xsl:copy-of select="*"/>
			<xsl:variable name="date" select="date:date()"/>
			<xsl:if test="$date">
				<refmiscinfo class="date">
					<xsl:value-of select="substring($date, 1, 10)"/>
				</refmiscinfo>
			</xsl:if>
		</xsl:copy>
	</xsl:template>

	<!-- copy everything else unchanged -->
	<xsl:template match="@*|node()">
		<xsl:copy>
			<xsl:apply-templates select="@*|node()"/>
		</xsl:copy>
	</xsl:template>
</xsl:stylesheet>
